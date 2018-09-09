#pragma once
#include "KDTrees.h"

namespace Rak {
	namespace Rendering {
		namespace Mesh {
			struct TriangleBoundingBox : Rak::Core::BoundingBox {
				unsigned int Index;
				unsigned int Index2;
			};
			using TriangleBoundingBoxEntry = std::vector<TriangleBoundingBox>;
			struct BVHNode : Rak::Core::BoundingBox {
				bool Leaf = false; 
				BVHNode * Left = nullptr, *Right = nullptr;
				TriangleBoundingBoxEntry LeafEntry;
				unsigned int StartIndex;
			};
			

			

			BVHNode * ConstructBoundingVolumeHieracy(Mesh & Mesh);

			BSTImageData StoreBVHInImage(BVHNode * Node, unsigned int Width, unsigned int Height); 

		}
	}
}




