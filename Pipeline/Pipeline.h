#pragma once 
#include <Pipeline\HybridRasterizing.h> 
#include <Pipeline\HybridPathTracing.h> 
#include <Rendering/GUISystem.h>



namespace Rak {
	namespace Rendering {
		namespace Combined {
			class Pipeline {
				Rasterizer::DefferedPipeLine Deffered;
				PathTracing::PathTracingPipeLine PathTracing;
				std::vector<Mesh::Model> Models;  
				Core::Shader CompositingShader;
				GUI::GUIPipeline GUI; 
			public: 
				Pipeline(); 
				void Prepare(std::vector<Mesh::Model> Models, Window & Window, Control::Camera & Camera);
				void Run(Window & Window, Control::Camera & Camera); 
				void Compositing(Control::Camera & Camera);
			}; 
		}
	}
}