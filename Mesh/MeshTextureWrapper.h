#pragma once
#include <Mesh/BVH.h>
#include <Dependencies\DependenciesRendering.h> 
#include <Mesh/Mesh.h>
#define MATERIAL_TEXTURE_QUALITY 256 //the texture maps' resolution (they all have to be the same) 
#define MATERIAL_TEXTURE_QUALITY_SQUARED MATERIAL_TEXTURE_QUALITY * MATERIAL_TEXTURE_QUALITY
//^ could probably be constants 
namespace Rak { 
	namespace Rendering {
		namespace Mesh {
			struct WrappedModel {
				unsigned int MeshData, KDTreeData; 
				Vector2i MeshDataResolution; 
				WrappedModel();
				MeshMaterials MaterialData; 
				bool Wrap(Model &WrapModel); 
			}; 
		}
	}
}