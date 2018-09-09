#pragma once
#include <Mesh\Mesh.h>
#include <Rendering\Shader.h>
#include <Rendering\FrameBuffer.h>
namespace Rak {
	namespace Rendering {

		struct ShadowStructure {
			Core::FrameBufferObject ShadowMaps[3];
			Matrix4f ViewMatrix[3], ProjectionMatrix[3];

			ShadowStructure();
			void PrepareShadowStructure(Vector2i Resolution);
			void UpdateRotation(Vector3f Direction, Control::Camera Camera);
			void BindUniform(unsigned int ShaderID, unsigned int BaseTexture);

		};
	}
}