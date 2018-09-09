#pragma once
#include <Mesh/BVH.h>
#include <Dependencies\DependenciesRendering.h> 
#include <Mesh/Mesh.h>

namespace Rak {
	namespace Rendering {
		namespace Mesh {
			struct Ray {
				Vector3f Direction, Origin, OneOverDirection;
			};
			void Select(Window & Window,const Control::Camera & Camera,const std::vector<Model> & Models, int & Model, int & Material); 
		}
	}
}