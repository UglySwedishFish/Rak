#pragma once
#include <Dependencies\DependenciesPathTracing.h> 
#include <Pipeline\HybridRasterizing.h> 
#include <Rendering/ShadowPass.h>

namespace Rak {
	namespace Rendering {
		namespace PathTracing {
			class PathTracingPipeLine {
			public: 
				Core::Shader TraceShader, TemporalShader, PreviousShader, LightCombinerShader, ShadowPass, SpatialFilter, EquirectangularToCubeMap;
				bool First = true; 
				std::vector<Mesh::WrappedModel> WrappedModels;
				Core::MultiPassFrameBufferObject Temporal, CheckerBoard, Previous, Trace;
				Core::FrameBufferObject Combined, SpatialFilteredTrace[2]; 
				Core::TextureCubeMap Enviroment; 

				/*
				the spatial filtering here is done before the temporal filtering, as the temporal filtering is done in full
				resolution and also incorporates the albedo value of the material, and as we obviously dont want to blur the 
				albedo value we need to do it before the temporal filtering. 
				this does however have the issue of not knowing how much sampling is going to be done at one point
				and thus it is not an "intellegent" filter. This could in the future be solved by making the spatial filter temporarly avare
				(i;e seeing how much temporal filtering will be done on a pixel within the spatial filter itself, though this does affect performance)
				*/
			
				ShadowStructure Shadows; 
				Vector3f SunDirection; 
				float Time = 0.f; 
				
				unsigned int WrappedModelsMaterials;
				PathTracingPipeLine(); 
				void Prepare(std::vector<Mesh::Model> &Models, Control::Camera & Camera, Window & Window);
				void ComputeTrace(Window & Window, Control::Camera & Camera, Rasterizer::DefferedPipeLine & Pipeline, std::vector<Mesh::Model> & Models, DebugInfo & DebugInfo);
				void UpdateMaterials(); 
				inline void ResetTemporalFiltering() {
					First = true;
				}
			}; 
		}
	}
}