#pragma once
#include <Mesh\Mesh.h> 
#include <Dependencies\DependenciesRendering.h> 
#include <array>
#include <Core/Core.h>

#define MIN_SMALL Vector3f(-100000.)
#define MAX_LARGE Vector3f(100000.0)

namespace Rak { 
	namespace Rendering {
		namespace Mesh {
			 
			struct KDTriangle {
				std::array<Vector3f, 3> Vertices; 
				int StartIndicie; 
				inline Vector3f Midpoint() {
					return (Vertices[0] + Vertices[1] + Vertices[2]) / 3.f; 
				}
				inline KDTriangle(Vector3f V1 = Vector3f(0.), Vector3f V2 = Vector3f(0.), Vector3f V3 = Vector3f(0.), unsigned int i = 0) : Vertices{V1, V2, V3}, StartIndicie(i) {}
				bool operator==(const KDTriangle & a) {
					return Vertices[0] == a.Vertices[0] && Vertices[1] == a.Vertices[1] && Vertices[2] == a.Vertices[2]; 
				}
			};

			inline std::vector<KDTriangle> ConvertFromVertices(std::vector<Vector3f> Vertices) {

				std::vector<KDTriangle> Tris; 

				for (int vert = 0; vert < Vertices.size(); vert += 3) {
					Tris.push_back(KDTriangle(Vertices[vert], Vertices[vert + 1], Vertices[vert + 2], vert)); 
				}

				return Tris; 

			}

			inline float GetSmallest(float a, float b) {
				return a > b ? b : a; 
			}
			inline float GetSmallest(float a, float b, float c) {
				return a < b && a < c ? a : b < c ? b : c; 
			}
			inline float GetLargest(float a, float b) {
				return a > b ? a : b;
			}
			inline float GetLargest(float a, float b, float c) {
				return a > b && a > c ? a : b > c ? b : c;
			}
			
			struct KDBoundingBox : Rak::Core::BoundingBox {
				inline void ResizeToFit(KDTriangle Triangle) {
					for (int i = 0; i < 3; i++) {
						Min[i] = GetSmallest(Min[i], GetSmallest(Triangle.Vertices[0][i], Triangle.Vertices[1][i], Triangle.Vertices[2][i]));
						Max[i] = GetLargest(Max[i], GetLargest(Triangle.Vertices[0][i], Triangle.Vertices[1][i], Triangle.Vertices[2][i]));
					}
				}
			};


			struct KDNode {
				KDBoundingBox BoundingBox; 
				KDNode *Left, *Right; 
				bool IsLeaf = false; 
				std::vector<KDTriangle> Triangles; 
			};
			KDNode * ConstructKDTreeFromModel(std::vector<KDTriangle> Tris, int depth = 0);
			struct BSTImageData {
				std::vector<Vector4f> ImageData; 
				void Construct(unsigned int Width, unsigned int Height) {
					ImageData = std::vector<Vector4f>(Width*Height, Vector4f(0.));
				}
			};
			BSTImageData StoreKDTreeInImage(KDNode * Node, unsigned int Width, unsigned int Height); //required for path tracing later down the line

		}
	}
}