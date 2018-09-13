
#include "ShadowPass.h"
#include <Core\Core.h>
#include <Rendering\FrameBuffer.h>
namespace Rak {
	namespace Rendering {
		ShadowStructure::ShadowStructure()
		{

		}

		void ShadowStructure::PrepareShadowStructure(Vector2i Resolution)
		{
			for (int ShadowProjection = 0; ShadowProjection < 3; ShadowProjection++) {
				ProjectionMatrix[ShadowProjection] = Rak::Core::ShadowOrthoMatrix(3.0 * pow(7.f, float(ShadowProjection)), 0.1, 100.);
				ShadowMaps[ShadowProjection] = Core::FrameBufferObject(Resolution, true, false);
			}
		}

		void ShadowStructure::UpdateRotation(Vector3f Direction, Control::Camera Camera)
		{

			Vector3f Orientation = Vector3f(0.0, sin(Direction.x * 0.0174532925), cos(Direction.x * 0.0174532925));

			for (int i = 0; i < 3; i++) {
				ViewMatrix[i] = Rak::Core::ViewMatrix(Orientation * 50.f, Direction);
			}

		}

		

		void ShadowStructure::BindUniform(unsigned int ShaderID, unsigned int BaseTexture) {

			for (int ShadowMap = 0; ShadowMap < 3; ShadowMap++) {
				std::string Base = "ShadowMaps[" + std::to_string(ShadowMap) + "].";

				std::string Map = Base + "Map";
				std::string ShadowView = Base + "ShadowView";
				std::string ShadowProj = Base + "ShadowProj";

				glActiveTexture(GL_TEXTURE0 + BaseTexture + ShadowMap);
				glBindTexture(GL_TEXTURE_2D, ShadowMaps[ShadowMap].DepthBuffer);
				glUniform1i(glGetUniformLocation(ShaderID, Map.c_str()), BaseTexture + ShadowMap);

				glUniformMatrix4fv(glGetUniformLocation(ShaderID, ShadowView.c_str()), 1, false, glm::value_ptr(ViewMatrix[ShadowMap]));
				glUniformMatrix4fv(glGetUniformLocation(ShaderID, ShadowProj.c_str()), 1, false, glm::value_ptr(ProjectionMatrix[ShadowMap]));

			}

		}

	}
}