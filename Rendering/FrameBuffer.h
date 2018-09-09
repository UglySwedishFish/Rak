#pragma once
#include <Dependencies\DependenciesRendering.h> 
#include <Dependencies\DependenciesMath.h> 
#include <Rendering\Window.h> 
#include <Mesh\Mesh.h> 

namespace Rak {
	namespace Rendering {
		namespace Core {
			struct FrameBufferObject {
				GLuint FrameBuffer, ColorBuffer, DepthBuffer;
				glm::ivec2 Resolution;
				bool GenerateMip; 
				FrameBufferObject(glm::ivec2 Resolution, bool SamplerShadow, bool alpha = false, bool generatemip = false);
				FrameBufferObject(); 
				void Bind();
				void UnBind(Window Window);

			};
			struct MultiPassFrameBufferObject {
				GLuint FrameBuffer, DepthBuffer;
				std::vector<GLuint> ColorBuffers;
				glm::ivec2 Resolution;
				bool GenerateMip; 
				MultiPassFrameBufferObject(glm::ivec2 Resolution, int stages, bool alpha = false, bool generatemip = false);
				MultiPassFrameBufferObject(); 
				void Bind();
				void UnBind(Window Window);
			};
			void PreparePostProcess();
			void DrawPostProcessQuad();
			void DrawPostProcessCube(); 
		}
	}
}