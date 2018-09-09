#include "BVH.h"
#include <iostream>

namespace Rak {
	namespace Rendering {
		namespace Mesh {
			
			std::vector<int> P; 

			const float Epsilon = 1e-3; 

			BVHNode * BuildRecursively(TriangleBoundingBoxEntry & Boxes, int &Index, int Depth = 0) {

				BVHNode * Node = new BVHNode;
				Node->Leaf = false; 
				

				//construct bounding box around
				for (auto & Box : Boxes) {
					for (int Axis = 0; Axis < 3; Axis++) {

						Node->Max[Axis] = std::max(Box.Max[Axis], Node->Max[Axis]);
						Node->Min[Axis] = std::min(Box.Min[Axis], Node->Min[Axis]); 

						

					}
				}

				for (int Axis = 0; Axis < 3; Axis++) {
					if (Node->Min[Axis] == Node->Max[Axis]) {
						float Value = Node->Min[Axis];

						Node->Min[Axis] = Value - Epsilon;
						Node->Max[Axis] = Value + Epsilon;
					}
				}

				Vector3f Size = Node->Size();
				float MinCost = Boxes.size() * (Size.x * Size.y * Size.z), BestSplit = -1.f; 
				int BestAxis = -1;
				//Mincost is that of not splitting at all 


				//SAH, tries to find the best possible split point and axis by sr*r + sl*l (its O(N) so its not too bad)

				auto GetCost = [](Vector3f RightSize, int CountRight, Vector3f LeftSize, int CountLeft) {return (RightSize.x*RightSize.y*RightSize.z) * CountRight + (LeftSize.x*LeftSize.y*LeftSize.z) * CountLeft; };

				for (int Axis = 0; Axis < 3; Axis++) {
					float Start = Node->Min[Axis], Stop = Node->Max[Axis]; //make the testing less (only to a certain point though, after depth 63 it clamps) 
					float Step = (Stop - Start) / (1024.f / std::min(float(Depth) + 1.f, 64.f)); 
					

					if (abs(Stop - Start) < 0.001) //this bounding box is too small, skip to next axis (unless this is the last axis) 
						continue; 
					
					for (float TestSplit = Start + Step; TestSplit < Stop - Step; TestSplit += Step) {
						
						Rak::Core::BoundingBox Right, Left; 

						int CountRight = 0, CountLeft = 0;

						for (auto & Box : Boxes) {
							float CenterAxis = Box.Center()[Axis];

							if (CenterAxis > TestSplit) { //right side
								Right.Max = glm::max(Right.Max, Box.Max);
								Right.Min = glm::min(Right.Min, Box.Min);
								CountRight++;
							}
							else {
								Left.Max = glm::max(Left.Max, Box.Max);
								Left.Min = glm::min(Left.Min, Box.Min);
								CountLeft++;
							}

						}

						if (Left.Min[Axis] == Left.Max[Axis]) {
							float Value = Left.Min[Axis];

							Left.Min[Axis] = Value - Epsilon;
							Left.Max[Axis] = Value + Epsilon;
						}
						if (Right.Min[Axis] == Right.Max[Axis]) {
							float Value = Left.Min[Axis];

							Right.Min[Axis] = Value - Epsilon;
							Right.Max[Axis] = Value + Epsilon;
						}
						

						
						//skip garbage splits 
						if (CountRight > 1 && CountLeft > 1) {
							Vector3f LeftSize = Left.Size();
							Vector3f RightSize = Right.Size();

							float Cost = GetCost(RightSize, CountRight, LeftSize, CountLeft);

							if (Cost < MinCost) { //if the cost is less than that of the min cost
								MinCost = Cost;
								BestAxis = Axis;
								BestSplit = TestSplit;
							}
						}

					}
				}

				if (BestAxis == -1 || Boxes.size() <= 4) { //no split was deemed faster than it would be to just turn this into a leaf. 

					BVHNode * Leaf = new BVHNode; 

					Leaf->Leaf = true;
					Leaf->StartIndex = Index;

					for (int Box = 0; Box < Boxes.size(); Box++) {
						Boxes[Box].Index2 = Index;
						
						Leaf->LeafEntry.push_back(Boxes[Box]);

						Index++;

					}
				/*	for (int i = 0; i < Boxes.size(); i++) {
						for (int Axis = 0; Axis < 3; Axis++) {
							Leaf->Max[Axis] = std::max(Boxes[i].Max[Axis], Node->Max[Axis]);
							Leaf->Min[Axis] = std::min(Boxes[i].Min[Axis], Node->Min[Axis]);
						}
					}*/
					return Leaf;
				}

				TriangleBoundingBoxEntry Left, Right; 
				Rak::Core::BoundingBox LeftBox, RightBox; 

				for (auto & Box : Boxes) {
					

					float CenterAxis = Box.Center()[BestAxis];

					if (CenterAxis > BestSplit) { //right side
						RightBox.Max = glm::max(RightBox.Max, Box.Max);
						RightBox.Min = glm::min(RightBox.Min, Box.Min);
						Right.push_back(Box); 
					}
					else {
						LeftBox.Max = glm::max(LeftBox.Max, Box.Max);
						LeftBox.Min = glm::min(LeftBox.Min, Box.Min);
						Left.push_back(Box); 
					}

				}

				for (int Axis = 0; Axis < 3; Axis++) {
					if (LeftBox.Min[Axis] == LeftBox.Max[Axis]) {
						float Value = LeftBox.Min[Axis];

						LeftBox.Min[Axis] = Value - Epsilon;
						LeftBox.Max[Axis] = Value + Epsilon;
					}
					if (RightBox.Min[Axis] == RightBox.Max[Axis]) {
						float Value = RightBox.Min[Axis];

						RightBox.Min[Axis] = Value - Epsilon;
						RightBox.Max[Axis] = Value + Epsilon;
					}
				}

				Node->Left = BuildRecursively(Left, Index, Depth + 1);
				Node->Left->Max = LeftBox.Max;
				Node->Left->Min = LeftBox.Min;



				Node->Right = BuildRecursively(Right, Index, Depth + 1);
				Node->Right->Max = RightBox.Max;
				Node->Right->Min = RightBox.Min;

				return Node;

			}

			void RecursivelySwapAround(BVHNode * Node, std::vector<Vector3f> &TempVertices, std::vector<Vector3f> &TempNormals, std::vector<Vector3f> &TempTexCoords, std::vector<Vector3f> &TempTangents, std::vector<Vector3f> &Vertices, std::vector<Vector3f> &Normals, std::vector<Vector3f> &TexCoords, std::vector<Vector3f> &Tangents) {
				

				if (Node->Leaf) {
					for (auto & Box : Node->LeafEntry) {
						for (int i = 0; i < 3; i++) {
							TempVertices[Box.Index2 * 3 + i] = Vertices[Box.Index + i]; 
							TempNormals[Box.Index2 * 3 + i] = Normals[Box.Index + i];
							TempTexCoords[Box.Index2 * 3 + i] = TexCoords[Box.Index + i];
							TempTangents[Box.Index2 * 3 + i] = Tangents[Box.Index + i];
						}
					}
				}
				else {
					RecursivelySwapAround(Node->Left, TempVertices, TempNormals, TempTexCoords, TempTangents, Vertices, Normals, TexCoords, Tangents); 					
					RecursivelySwapAround(Node->Right, TempVertices, TempNormals, TempTexCoords, TempTangents, Vertices, Normals, TexCoords, Tangents);
				}


			}

			BVHNode * ConstructBoundingVolumeHieracy(Mesh & Mesh)	{
				TriangleBoundingBoxEntry BaseEntry; 
				BVHNode * Node = new BVHNode;
				

				for (int Triangle = 0; Triangle < Mesh.Vertices.size(); Triangle += 3) {
					TriangleBoundingBox TemporaryEntry; 
					for (int Axis = 0; Axis < 3; Axis++) {
						TemporaryEntry.Max[Axis] = Rak::Core::max3(Mesh.Vertices[Triangle][Axis], Mesh.Vertices[Triangle + 1][Axis], Mesh.Vertices[Triangle + 2][Axis]);
						TemporaryEntry.Min[Axis] = Rak::Core::min3(Mesh.Vertices[Triangle][Axis], Mesh.Vertices[Triangle + 1][Axis], Mesh.Vertices[Triangle + 2][Axis]);
						//Node->Max[Axis] = Rak::Core::max4(Node->Max[Axis], Mesh.Vertices[Triangle][Axis], Mesh.Vertices[Triangle + 1][Axis], Mesh.Vertices[Triangle + 2][Axis]);
						//Node->Min[Axis] = Rak::Core::min4(Node->Max[Axis], Mesh.Vertices[Triangle][Axis], Mesh.Vertices[Triangle + 1][Axis], Mesh.Vertices[Triangle + 2][Axis]);

					}
					TemporaryEntry.Index = Triangle; 
					BaseEntry.push_back(TemporaryEntry); 
				}

				int Index = 0; 

				Node = BuildRecursively(BaseEntry, Index);

				std::cout << "Final Index: " << Index << " BaseEntry: " << BaseEntry.size() << '\n'; 

				for (int i = 0; i < P.size(); i++) {
					for (int j = 0; j < P.size(); j++) {
						if (j != i) {
							if (P[i] == P[j])
								std::cout << "Duplicates found, " << j << " " << i << '\n'; 
						}
					}
				}
				
				//now, swap around vertices, normals, texcoords 

				std::vector<Vector3f> TempVertices, TempNormals, TempTexCoords, TempTangents; 
				TempVertices.resize(Mesh.Vertices.size()); 
				TempNormals.resize(Mesh.Vertices.size());
				TempTexCoords.resize(Mesh.Vertices.size());
				TempTangents.resize(Mesh.Vertices.size());

				RecursivelySwapAround(Node, TempVertices, TempNormals, TempTexCoords, TempTangents, Mesh.Vertices, Mesh.Normals, Mesh.TextureCoordinates, Mesh.Tangents);
				//^ this makes the storing for the bvh much smaller 
				Mesh.Vertices = TempVertices; 
				Mesh.Normals = TempNormals; 
				Mesh.TextureCoordinates = TempTexCoords; 
				Mesh.Tangents = TempTangents; 

				return Node; 


			}

			float ToFloat(int a) {
				return glm::intBitsToFloat(a);
			}

			void SetPixelData(BSTImageData & Data, BVHNode * Node, unsigned int &PixelOffset) {
				if (Node->Leaf) {

					Vector4f Pixel1 = Vector4f(Node->Min, glm::intBitsToFloat(Node->StartIndex));
					Vector4f Pixel2 = Vector4f(Node->Max, glm::intBitsToFloat(Node->LeafEntry.size()));

					Data.ImageData[PixelOffset] = Pixel1;
					Data.ImageData[PixelOffset + 1] = Pixel2;

					PixelOffset += 2;
					
					
					 
				}
				else {


					Vector4f Pixel1 = Vector4f(Node->Min, 0.f);
					Vector4f Pixel2 = Vector4f(Node->Max, -1.f);

					Data.ImageData[PixelOffset] = Pixel1;
					Data.ImageData[PixelOffset + 1] = Pixel2;



					unsigned int Parent = PixelOffset;

					PixelOffset += 2;

					SetPixelData(Data, Node->Left, PixelOffset);
					Data.ImageData[Parent].a = glm::intBitsToFloat(PixelOffset); //index to right child 

					SetPixelData(Data, Node->Right, PixelOffset);

				}
			}

			BSTImageData StoreBVHInImage(BVHNode * Node, unsigned int Width, unsigned int Height)
			{
				
				BSTImageData Result;

				Result.Construct(Width, Height);
				unsigned int Offset = 0;
				SetPixelData(Result, Node, Offset); //store the data in a semi-gpu-friendly-way
				return Result;

			}



		}
	}
}