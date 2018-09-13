#include "HybridRasterizing.h"
#include <iostream> 

//simple 



void Rak::Rendering::Rasterizer::DefferedPipeLine::DrawDeffered(std::vector<Mesh::Model> & BaseModels, Control::Camera & Camera, Window & Window, DebugInfo & DebugInfo)
{

	sf::Clock DefferedTime; 

	DefferedFrameBuffer.Bind(); 
	Deffered.Bind(); 

	glUniformMatrix4fv(glGetUniformLocation(Deffered.ShaderID, "ViewMatrix"), 1, false, glm::value_ptr(Camera.View));
	glUniformMatrix4fv(glGetUniformLocation(Deffered.ShaderID, "ProjectionMatrix"), 1, false, glm::value_ptr(Camera.Project));

	BindTextures(3); 

	for (auto & Model : BaseModels) {
		Mesh::DrawModel(Model, Deffered, Camera, Window); //draw the model itself 
	}

	Deffered.UnBind(); 

	DefferedFrameBuffer.UnBind(Window); 

	#ifdef RAK_DEBUG 
	glFinish(); 
	#endif 

	DebugInfo.DefferedTime = DefferedTime.getElapsedTime().asSeconds() * 1000.f; 

}

void Rak::Rendering::Rasterizer::DefferedPipeLine::Prepare(Window & Screen)
{
	//Multi pass frame buffer object

	DefferedFrameBuffer = Core::MultiPassFrameBufferObject(Screen.GetResolution(), 3, true, true); //simple deffered framebuffer 
	Deffered = Core::Shader("Shaders/Deffered/vert.glsl", "Shaders/Deffered/frag.glsl"); //simple deffered shader 

	Deffered.Bind(); 

	SetUniforms(Deffered.ShaderID, 3); 

	Deffered.UnBind(); 
}

Rak::Rendering::Rasterizer::DefferedPipeLine::DefferedPipeLine() : Deffered(), DefferedFrameBuffer()
{
	
}
