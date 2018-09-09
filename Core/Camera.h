#pragma once

#include <Core\Core.h> 
#include <Rendering\Window.h> 

namespace Rak {
	namespace Control {
		struct Camera {
			Matrix4f Project, View, PrevView;
			Vector3f Position, Rotation;
			bool ShadowCamera;
			inline Camera(float fov, float znear, float zfar, Vector3f pos, Vector3f rot, Window & screen) :
				Project(glm::perspective(glm::radians(fov), float(screen.GetResolution().x) / float(screen.GetResolution().y), znear, zfar)),
				Position(pos),
				Rotation(rot),
				View(Rak::Core::ViewMatrix(pos, rot)),
				ShadowCamera(false)
			{}
			inline Camera() :
				Project(Matrix4f(0.)),
				View(Matrix4f(0.)),
				PrevView(Matrix4f(0.)),
				Position(Vector3f(0.)),
				Rotation(Vector3f(0.)),
				ShadowCamera(false)
			{

			}
			inline void SetPosition(Vector3f Position) {
				PrevView = View;
				View = Rak::Core::ViewMatrix(Position, Rotation);
			}
			inline void SetRotation(Vector3f Rotation) {
				PrevView = View;
				View = Rak::Core::ViewMatrix(Position, Rotation);
			}
			inline void Move(Vector3f PositionAddon) {
				PrevView = View;
				View = Rak::Core::ViewMatrix(Position + PositionAddon, Rotation);
			}
			inline void Rotate(Vector3f RotationAddon) {
				PrevView = View;
				View = Rak::Core::ViewMatrix(Position, Rotation + RotationAddon);
			}
		};
	}
}
