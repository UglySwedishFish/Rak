#include "HybridPathTracing.h"
#include <iostream> 

Rak::Rendering::PathTracing::PathTracingPipeLine::PathTracingPipeLine() 
	: WrappedModelsMaterials(0), Trace(), TraceShader(), Temporal(), TemporalShader(), Previous(), PreviousShader(), WrappedModels{}, Shadows(), SunDirection(0.)
{

}

void Rak::Rendering::PathTracing::PathTracingPipeLine::Prepare(std::vector<Mesh::Model> &Models, Control::Camera & Camera,Window & Window)
{
	

	//wrap all of the models materials into a 1d array texture

	

	//^ and at that moment the models are now locked into place, they can be moved with model matrices but nothing else	

	TraceShader = Core::Shader("Shaders/PathTracer/vert.glsl", "Shaders/PathTracer/frag.glsl");
	TemporalShader = Core::Shader("Shaders/Temporal/vert.glsl", "Shaders/Temporal/frag.glsl");
	PreviousShader = Core::Shader("Shaders/Previous/vert.glsl", "Shaders/Previous/frag.glsl");
	LightCombinerShader = Core::Shader("Shaders/LightCombiner/vert.glsl", "Shaders/LightCombiner/frag.glsl"); 
	ShadowPass = Core::Shader("Shaders/ShadowPass/vert.glsl", "Shaders/ShadowPass/frag.glsl"); 
	SpatialFilter = Core::Shader("Shaders/Spatial/vert.glsl", "Shaders/Spatial/frag.glsl"); 
	EquirectangularToCubeMap = Core::Shader("Shaders/EquirectangularToCubeMapShader/vert.glsl", "Shaders/EquirectangularToCubeMapShader/frag.glsl"); 
	Enviroment = Core::LoadHDRI("Resources/hdr.hdr", true, false, EquirectangularToCubeMap);

	Trace = Core::MultiPassFrameBufferObject(Window.GetResolution() / 2, 6, true); //handles the raw tracing 
	Temporal = Core::MultiPassFrameBufferObject(Window.GetResolution(), 3, false, true); //temporaly filters the upscaled version of the trace 
	Previous = Core::MultiPassFrameBufferObject(Window.GetResolution(), 8, true); //stores the required previous data. 
	Combined = Core::FrameBufferObject(Window.GetResolution(), false, true); //upscales the result from the trace 


	//spatials filters the raw trace, before it is temporaly filter
	SpatialFilteredTrace[0] = Core::FrameBufferObject(Window.GetResolution() / 2, false, true);
	SpatialFilteredTrace[1] = Core::FrameBufferObject(Window.GetResolution() / 2, false, true);
	

	for (auto Model : Models) {
		WrappedModels.push_back(Mesh::WrappedModel());
		WrappedModels[WrappedModels.size() - 1].Wrap(Model);
	}

	//do static uniforms in a static way (clever I know) 

	TraceShader.Bind(); 

	glUniform1i(glGetUniformLocation(TraceShader.ShaderID, "TotalModels"), static_cast<int>(Models.size())); 
	
	for (int Model = 0; Model < Models.size(); Model++) {
		

		std::string Addon = "[" + std::to_string(Model) + "]"; 

		std::string ModelData = "ModelData" + Addon;
		std::string VerticeCount = "VerticeCount" + Addon;
		std::string Resolution = "Resolution" + Addon;


		glUniform1i(glGetUniformLocation(TraceShader.ShaderID, ModelData.c_str()), Model+9);
		glUniform1i(glGetUniformLocation(TraceShader.ShaderID, VerticeCount.c_str()), Models[Model].ModelData.Vertices.size());
		glUniform2i(glGetUniformLocation(TraceShader.ShaderID, Resolution.c_str()), WrappedModels[Model].MeshDataResolution.x, WrappedModels[Model].MeshDataResolution.y);

	}

	glUniform1i(glGetUniformLocation(TraceShader.ShaderID, "PositionSpecular"), 0);
	glUniform1i(glGetUniformLocation(TraceShader.ShaderID, "NormalRefractive"), 1);
	glUniform1i(glGetUniformLocation(TraceShader.ShaderID, "IOREmmisisionRoughness"), 2);
	glUniform1i(glGetUniformLocation(TraceShader.ShaderID, "PreviousHit"), 3);
	glUniform1i(glGetUniformLocation(TraceShader.ShaderID, "PreviousDirection"), 4);
	glUniform1i(glGetUniformLocation(TraceShader.ShaderID, "PreviousIndirectHit"), 5);
	glUniform1i(glGetUniformLocation(TraceShader.ShaderID, "PreviousNormalHit"), 6);
	glUniform1i(glGetUniformLocation(TraceShader.ShaderID, "ModelMaterials"), 7);
	glUniform1i(glGetUniformLocation(TraceShader.ShaderID, "LowResTextures"), 8);
	glUniform1i(glGetUniformLocation(TraceShader.ShaderID, "DirectionalShadowMap"), 28);
	glUniform1i(glGetUniformLocation(TraceShader.ShaderID, "Enviroment"), 29);


	TraceShader.UnBind(); 

	TemporalShader.Bind(); 

	glUniform1i(glGetUniformLocation(TemporalShader.ShaderID, "PreviousSampling"), 0);
	glUniform1i(glGetUniformLocation(TemporalShader.ShaderID, "PreviousWorldPos"), 1);
	glUniform1i(glGetUniformLocation(TemporalShader.ShaderID, "PreviousTemporal"), 3);
	glUniform1i(glGetUniformLocation(TemporalShader.ShaderID, "PreviousRawTrace"), 2);
	glUniform1i(glGetUniformLocation(TemporalShader.ShaderID, "Trace"), 4);
	glUniform1i(glGetUniformLocation(TemporalShader.ShaderID, "WorldPosMetallic"), 5);
	glUniform1i(glGetUniformLocation(TemporalShader.ShaderID, "Albedo"), 6);

	TemporalShader.UnBind();

	PreviousShader.Bind(); 

	glUniform1i(glGetUniformLocation(PreviousShader.ShaderID, "Position"), 0);
	glUniform1i(glGetUniformLocation(PreviousShader.ShaderID, "Samples"), 1);
	glUniform1i(glGetUniformLocation(PreviousShader.ShaderID, "RawTrace"), 2);
	glUniform1i(glGetUniformLocation(PreviousShader.ShaderID, "TemporalTrace"), 3);
	glUniform1i(glGetUniformLocation(PreviousShader.ShaderID, "Albedo"), 4);
	glUniform1i(glGetUniformLocation(PreviousShader.ShaderID, "Hit"), 5);
	glUniform1i(glGetUniformLocation(PreviousShader.ShaderID, "Direction"), 6);
	glUniform1i(glGetUniformLocation(PreviousShader.ShaderID, "Indirect"), 7);
	glUniform1i(glGetUniformLocation(PreviousShader.ShaderID, "Normal"), 8);

	
	PreviousShader.UnBind();

	LightCombinerShader.Bind(); 

	glUniform1i(glGetUniformLocation(LightCombinerShader.ShaderID, "RawTrace"), 0);
	glUniform1i(glGetUniformLocation(LightCombinerShader.ShaderID, "HalfResPosition"), 1);
	glUniform1i(glGetUniformLocation(LightCombinerShader.ShaderID, "PositionMetallic"), 2);

	LightCombinerShader.UnBind(); 

	SpatialFilter.Bind(); 

	glUniform1i(glGetUniformLocation(SpatialFilter.ShaderID, "RawInput"), 0);
	glUniform1i(glGetUniformLocation(SpatialFilter.ShaderID, "NormalRoughness"), 1);
	glUniform1i(glGetUniformLocation(SpatialFilter.ShaderID, "PositionMetallic"), 2);

	SpatialFilter.UnBind(); 

	glGenTextures(1, &WrappedModelsMaterials);
	glBindTexture(GL_TEXTURE_2D, WrappedModelsMaterials);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
	UpdateMaterials();

	Shadows.PrepareShadowStructure(Vector2i(1024)); 

	

}
void Rak::Rendering::PathTracing::PathTracingPipeLine::ComputeTrace(Window & Window, Control::Camera & Camera, Rasterizer::DefferedPipeLine & Pipeline, std::vector<Mesh::Model> & Models, DebugInfo & DebugInfo)
{

	sf::Clock DebugTime; 

	SunDirection = Vector3f(0., sin(Time * (3.1415926535 / 180.0)), cos(Time * (3.1415926535 / 180.0)));

	Vector3f ShadowRotation = Vector3f(Time, 0.0, 0.0);

	Shadows.UpdateRotation(ShadowRotation, Camera); 

	ShadowPass.Bind(); 

	Shadows.ShadowMaps[0].Bind(); 

	glUniformMatrix4fv(glGetUniformLocation(ShadowPass.ShaderID, "ProjectionMatrix"), 1, false, glm::value_ptr(Shadows.ProjectionMatrix[0])); 
	glUniformMatrix4fv(glGetUniformLocation(ShadowPass.ShaderID, "ViewMatrix"), 1, false, glm::value_ptr(Shadows.ViewMatrix[0]));

	for (auto & Model : Models) {
		Mesh::DrawModel(Model, ShadowPass, Camera, Window);
	}

	Shadows.ShadowMaps[0].UnBind(Window); 

	ShadowPass.UnBind();

	#ifdef RAK_DEBUG 
	glFinish(); 
	#endif 
	DebugInfo.ShadowPassTime = DebugTime.getElapsedTime().asSeconds() * 1000.f; 
	DebugTime.restart(); 


	Trace.Bind(); 

	TraceShader.Bind(); 

	for (int Model = 0; Model < WrappedModels.size(); Model++) {

		glActiveTexture(GL_TEXTURE9 + Model);
		glBindTexture(GL_TEXTURE_2D_ARRAY, WrappedModels[Model].MeshData);
		
	}


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Pipeline.DefferedFrameBuffer.ColorBuffers[2]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Pipeline.DefferedFrameBuffer.ColorBuffers[1]);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, Pipeline.DefferedFrameBuffer.ColorBuffers[3]);

	for (int i = 0; i < 4; i++) {
		glActiveTexture(GL_TEXTURE3+i);
		glBindTexture(GL_TEXTURE_2D, Previous.ColorBuffers[i+4]);
	}

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, WrappedModelsMaterials);

	BindTextureLowRes(8); 

	glActiveTexture(GL_TEXTURE28);
	glBindTexture(GL_TEXTURE_2D, Shadows.ShadowMaps[0].DepthBuffer);
	Enviroment.Bind(29); 

	glUniformMatrix4fv(glGetUniformLocation(TraceShader.ShaderID, "ViewMatrix"), 1, false, glm::value_ptr(Camera.View));
	glUniformMatrix4fv(glGetUniformLocation(TraceShader.ShaderID, "ProjectionMatrix"), 1, false, glm::value_ptr(Camera.Project));
	glUniform3f(glGetUniformLocation(TraceShader.ShaderID, "CameraPosition"), Camera.Position.x, Camera.Position.y, Camera.Position.z);
	glUniform1i(glGetUniformLocation(TraceShader.ShaderID, "Frame"), Window.GetFrameCount()%10000+1);
	glUniform1i(glGetUniformLocation(TraceShader.ShaderID, "CheckerBoard"), Window.GetFrameCount()%2);
	glUniformMatrix4fv(glGetUniformLocation(TraceShader.ShaderID, "DirectionalProjectionMatrix"), 1, false, glm::value_ptr(Shadows.ProjectionMatrix[0]));
	glUniformMatrix4fv(glGetUniformLocation(TraceShader.ShaderID, "DirectionalViewMatrix"), 1, false, glm::value_ptr(Shadows.ViewMatrix[0]));
	glUniform3f(glGetUniformLocation(TraceShader.ShaderID, "SunDirection"), SunDirection.x, SunDirection.y, SunDirection.z);


	//^ those are dynamic uniforms 

	Core::DrawPostProcessQuad(); //draw the "post processing" quad (i;e the quad that fills the entire display) 

	TraceShader.UnBind(); 

	Trace.UnBind(Window);

	#ifdef RAK_DEBUG 
	glFinish(); 
#endif
	DebugInfo.TraceTime = DebugTime.getElapsedTime().asSeconds() * 1000.f;
	DebugTime.restart();

	SpatialFilter.Bind(); 

	glUniformMatrix4fv(glGetUniformLocation(SpatialFilter.ShaderID, "ViewMatrix"), 1, false, glm::value_ptr(Camera.View));
	glUniformMatrix4fv(glGetUniformLocation(SpatialFilter.ShaderID, "ProjectionMatrix"), 1, false, glm::value_ptr(Camera.Project));

	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, Trace.ColorBuffers[0]); 

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Pipeline.DefferedFrameBuffer.ColorBuffers[1]);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, Pipeline.DefferedFrameBuffer.ColorBuffers[2]);
	glUniform1i(glGetUniformLocation(SpatialFilter.ShaderID, "First"), First);


	for (int i = 0; i < 2; i++) {

		SpatialFilteredTrace[i].Bind(); 

		glUniform1i(glGetUniformLocation(SpatialFilter.ShaderID, "Vertical"), i); 

		Core::DrawPostProcessQuad(); 

		SpatialFilteredTrace[i].UnBind(Window);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, SpatialFilteredTrace[0].ColorBuffer);
	}

	SpatialFilter.UnBind(); 

	#ifdef RAK_DEBUG 
	glFinish(); 
#endif
	DebugInfo.SpatialTime = DebugTime.getElapsedTime().asSeconds() * 1000.f;
	DebugTime.restart();

	Temporal.Bind(); 

	TemporalShader.Bind();

	glUniformMatrix4fv(glGetUniformLocation(TemporalShader.ShaderID, "PreviousViewMatrix"), 1, false, glm::value_ptr(Camera.PrevView));
	glUniformMatrix4fv(glGetUniformLocation(TemporalShader.ShaderID, "ViewMatrix"), 1, false, glm::value_ptr(Camera.View));
	glUniformMatrix4fv(glGetUniformLocation(TemporalShader.ShaderID, "ProjectionMatrix"), 1, false, glm::value_ptr(Camera.Project));
	glUniform1i(glGetUniformLocation(TemporalShader.ShaderID, "CheckerBoard"), Window.GetFrameCount() % 2);

	glUniform1i(glGetUniformLocation(TemporalShader.ShaderID, "First"), First);

	/*
	glUniform1i(glGetUniformLocation(TemporalShader.ShaderID, "PreviousSampling"), 0);
	glUniform1i(glGetUniformLocation(TemporalShader.ShaderID, "PreviousWorldPos"), 1);
	glUniform1i(glGetUniformLocation(TemporalShader.ShaderID, "PreviousTemporal"), 3);
	glUniform1i(glGetUniformLocation(TemporalShader.ShaderID, "PreviousRawTrace"), 2);
	glUniform1i(glGetUniformLocation(TemporalShader.ShaderID, "Trace"), 4);
	glUniform1i(glGetUniformLocation(TemporalShader.ShaderID, "WorldPosMetallic"), 5);
	glUniform1i(glGetUniformLocation(TemporalShader.ShaderID, "Albedo"), 6);
	*/

	for (int i = 0; i < 4; i++) {
		glActiveTexture(GL_TEXTURE0 + i); 
		glBindTexture(GL_TEXTURE_2D, Previous.ColorBuffers[i]); 
	}

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, Combined.ColorBuffer);
	
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, Pipeline.DefferedFrameBuffer.ColorBuffers[2]);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, Pipeline.DefferedFrameBuffer.ColorBuffers[0]);



	Core::DrawPostProcessQuad(); 

	TemporalShader.UnBind(); 

	Temporal.UnBind(Window); 

	#ifdef RAK_DEBUG 
	glFinish(); 
	#endif
	DebugInfo.TemporalTime = DebugTime.getElapsedTime().asSeconds() * 1000.f;
	DebugTime.restart();

	

	Combined.Bind(); 

	LightCombinerShader.Bind(); 

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, SpatialFilteredTrace[1].ColorBuffer);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Trace.ColorBuffers[1]);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, Pipeline.DefferedFrameBuffer.ColorBuffers[2]);

	glUniform3f(glGetUniformLocation(LightCombinerShader.ShaderID, "CameraPosition"), Camera.Position.x, Camera.Position.y, Camera.Position.z);

	Core::DrawPostProcessQuad(); 

	LightCombinerShader.Bind();

	Combined.UnBind(Window); 

	#ifdef RAK_DEBUG 
	glFinish(); 
#endif
	DebugInfo.LightCombinerTime = DebugTime.getElapsedTime().asSeconds() * 1000.f;
	DebugTime.restart();

	/*
	glUniform1i(glGetUniformLocation(PreviousShader.ShaderID, "Position"), 0);
	glUniform1i(glGetUniformLocation(PreviousShader.ShaderID, "Samples"), 1);
	glUniform1i(glGetUniformLocation(PreviousShader.ShaderID, "RawTrace"), 2);
	glUniform1i(glGetUniformLocation(PreviousShader.ShaderID, "TemporalTrace"), 3);
	glUniform1i(glGetUniformLocation(PreviousShader.ShaderID, "Albedo"), 4);
	*/

	Previous.Bind();

	PreviousShader.Bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Pipeline.DefferedFrameBuffer.ColorBuffers[2]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Temporal.ColorBuffers[0]);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, Combined.ColorBuffer);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, Temporal.ColorBuffers[1]);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, Pipeline.DefferedFrameBuffer.ColorBuffers[0]);

	for (int i = 0; i < 4; i++) {
		glActiveTexture(GL_TEXTURE5+i);
		glBindTexture(GL_TEXTURE_2D, Trace.ColorBuffers[i+2]);
	}

	Core::DrawPostProcessQuad();

	PreviousShader.UnBind();

	Previous.UnBind(Window);

	#ifdef RAK_DEBUG 
	glFinish();
	#endif
	DebugInfo.TemporalTime += DebugTime.getElapsedTime().asSeconds() * 1000.f;
	DebugTime.restart();

	First = false; 
}
void Rak::Rendering::PathTracing::PathTracingPipeLine::UpdateMaterials()
{

	glBindTexture(GL_TEXTURE_2D, WrappedModelsMaterials); 

	int MAX_MATERIALS = 1; 
	for (auto & Model : WrappedModels) {
		if (Model.MaterialData.Materials.size() > MAX_MATERIALS)
			MAX_MATERIALS = Model.MaterialData.Materials.size(); 
	}

	std::cout << MAX_MATERIALS << '\n'; 

	std::vector<Vector3f> Data(WrappedModels.size() * MAX_MATERIALS * 2);

	for (int WrapModel = 0; WrapModel < WrappedModels.size(); WrapModel++) {
		

		if (WrappedModels[WrapModel].MaterialData.Materials.size() > MAX_MATERIALS) {
			assert("Materialstructure too large!"); 
		}

		for (int Material = 0; Material < WrappedModels[WrapModel].MaterialData.Materials.size() * 2; Material+=2) {
			Data[WrapModel * MAX_MATERIALS + Material] = WrappedModels[WrapModel].MaterialData.Materials[Material / 2].AlbedoMultiplier;
			switch (WrappedModels[WrapModel].MaterialData.Materials[Material / 2].WorkFlow) {
			case 0:
				Data[WrapModel * MAX_MATERIALS + Material + 1] = glm::vec3(WrappedModels[WrapModel].MaterialData.Materials[Material / 2].MaterialZoom, glm::intBitsToFloat(WrappedModels[WrapModel].MaterialData.Materials[Material / 2].Texture), -1.f);
				break;
			case 1:
				Data[WrapModel * MAX_MATERIALS + Material + 1] = glm::vec3(0., WrappedModels[WrapModel].MaterialData.Materials[Material / 2].ValueTwo.y, WrappedModels[WrapModel].MaterialData.Materials[Material / 2].ValueTwo.z);
				break;
			case 2:
				Data[WrapModel * MAX_MATERIALS + Material + 1] = glm::vec3(0.f, WrappedModels[WrapModel].MaterialData.Materials[Material / 2].ValueTwo.y, -20.f);
				break;
			}
		}
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, MAX_MATERIALS * 2, WrappedModels.size(), 0, GL_RGB, GL_FLOAT, &Data[0]);

	glBindTexture(GL_TEXTURE_2D, 0);

	std::cout << WrappedModelsMaterials << '\n'; 
}
