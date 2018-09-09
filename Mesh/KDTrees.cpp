#include "KDTrees.h"
#include <iostream>



namespace Rak {
	namespace Rendering {
		namespace Mesh {

			struct KDTriangleLongestAxis {
				std::array<float, 3> Vertices; 
				KDTriangleLongestAxis(float v1 = 0., float v2 = 0., float v3 = 0.) : Vertices {v1,v2,v3} {}
				bool operator<(KDTriangleLongestAxis A) {
					return AvgPoint() < A.AvgPoint(); 
				}
				bool operator>(KDTriangleLongestAxis A) {
					return AvgPoint() > A.AvgPoint();
				}
				float AvgPoint() {
					return (Vertices[0] + Vertices[1] + Vertices[2])/3.f; 
				}
			};
			struct KDTriangleLongestAxisWithIndicie {
				std::array<float, 3> Vertices;
				int Index; 
				KDTriangleLongestAxisWithIndicie(float v1 = 0., float v2 = 0., float v3 = 0., int index = 0) : Vertices{ v1,v2,v3 }, Index(index) {}
				bool operator<(KDTriangleLongestAxisWithIndicie A) {
					return AvgPoint() < A.AvgPoint();
				}
				bool operator>(KDTriangleLongestAxisWithIndicie A) {
					return AvgPoint() > A.AvgPoint();
				}
				float AvgPoint() {
					return (Vertices[0] + Vertices[1] + Vertices[2]) / 3.f;
				}
			};
			
			void MedianSplit(std::vector<KDTriangle> &RightChild, std::vector<KDTriangle> &LeftChild, std::vector<KDTriangle> &Current, KDBoundingBox BoundingBox) {
				//first of all, find the longest axis
				int LongestAxis = BoundingBox.LongestAxis(); 

				//create a float vector containing the dimension of the current triangles longest axis' 

				std::vector<KDTriangleLongestAxis> CurrentLongestAxis;
				

				for (const auto & Triangle : Current) {
					CurrentLongestAxis.push_back(KDTriangleLongestAxis(Triangle.Vertices[0][LongestAxis], Triangle.Vertices[1][LongestAxis], Triangle.Vertices[2][LongestAxis])); 
				}


				
				//std::sort(CurrentLongestAxis.begin(), CurrentLongestAxis.end()); 

				//so, have now sorted the things 

				float MedianPoint = CurrentLongestAxis[static_cast<int>(floor(CurrentLongestAxis.size() / 2))].AvgPoint(); 
				//CurrentLongestAxis.clear(); 
				//we get the average point of the middle triangle 

				//now do the usual thing 

				for (int i = 0; i < Current.size(); i++)
					MedianPoint >= Current[i].Midpoint()[LongestAxis] ? RightChild.push_back(Current[i]) : LeftChild.push_back(Current[i]);
			}

			float Average(std::vector<KDTriangle> & Tris, int Axis) {
				Vector3f MidPoint = Vector3f(0.); 
				for (int i = 0; i < Tris.size(); i++) {
					MidPoint += Tris[i].Midpoint() * (1.f / Tris.size());
				}
				return MidPoint[Axis]; 
			}

			float Median(std::vector<KDTriangle> & Tris, int Axis) {
				std::vector<KDTriangleLongestAxis> NewTris; 
				for (auto Tri : Tris) {
					NewTris.push_back(KDTriangleLongestAxis(Tri.Vertices[0][Axis], Tri.Vertices[1][Axis], Tri.Vertices[2][Axis])); 
				}
				
				std::sort(NewTris.begin(), NewTris.end()); 
				for (auto Tri : NewTris) {
					//std::cout << Tri.AvgPoint() << '\n';
				}
				
				

				if (Tris.size() % 2 == 0) {
					return (NewTris[Tris.size() / 2 - 1].AvgPoint() + NewTris[Tris.size() / 2].AvgPoint()) / 2;
				}
				else {
					return NewTris[Tris.size() / 2].AvgPoint(); 
				}

			}

		

			float SAH(std::vector<KDTriangle> & Tris, int Axis) {
				//tries to fit as many triangles in as small of a place as possible 




			}
			float Middle(int Axis, KDBoundingBox Box) {
				return (Box.Min + Box.Max)[Axis] / 2.f;
			}


			float ChooseSolution(float Median, float Average, float Middle, std::vector<KDTriangle> Triangles, KDBoundingBox Box, int Axis) {


				int MedianRight = 0, Medianleft = 0, AverageRight = 0, AverageLeft = 0, MiddleRight = 0, MiddleLeft = 0; 

				for (int i = 0; i < Triangles.size(); i++) {
					Median >= Triangles[i].Midpoint()[Axis] ? MedianRight++ : Medianleft++;
					Average >= Triangles[i].Midpoint()[Axis] ? AverageRight++ : AverageLeft++;
					Middle >= Triangles[i].Midpoint()[Axis] ? MiddleRight++ : MiddleLeft++;
				}

				float InterpolatedMedian = (Median - Box.Min[Axis]) / (Box.Max[Axis] - Box.Min[Axis]); 
				float InterpolatedAverage = (Average - Box.Min[Axis]) / (Box.Max[Axis] - Box.Min[Axis]);
				float InterpolatedMiddle = (Middle - Box.Min[Axis]) / (Box.Max[Axis] - Box.Min[Axis]);;
				//interpolatedmiddle should actually always be 0.5 :thunk: 
				
				float CostMedian = 1. + ((Medianleft*Medianleft) * InterpolatedMedian) + ((MedianRight*MedianRight) * (1. - InterpolatedMedian));
				float CostAverage = 1. + ((AverageLeft*AverageLeft) * InterpolatedAverage) + ((AverageRight*AverageRight) * (1. - InterpolatedAverage));
				float CostMiddle = 1. + (MiddleLeft * InterpolatedMiddle) + (MiddleRight * (1. - InterpolatedMiddle));

				/*if (CostMedian < CostAverage && CostMedian < CostMiddle)
					std::cout << "Median\n"; 
				if (CostAverage < CostMedian && CostAverage < CostMiddle)
					std::cout << "Average\n";
				if (CostMiddle < CostAverage && CostMiddle < CostMedian)
					std::cout << "Middle\n";

				std::cout << "...\n\n"; */

				//return CostMedian < CostAverage && CostMedian < CostMiddle ? Median : Average;

				bool AllowMiddle = (MiddleRight > MiddleLeft ? (MiddleRight / 64 < MiddleLeft) : (MiddleRight / 64 > MiddleLeft));

				/*if (AlowMedian && (CostMedian < CostAverage || !AlowAverage) && (CostMedian < CostMiddle || !AlowMiddle))
					return Median;
				else if (AlowMiddle && (CostMiddle < CostAverage || !AlowAverage))
					return Middle;
				else
					return Average; 

					*/
				if (AllowMiddle && CostMiddle < CostAverage && CostMiddle < CostMedian)
					return Middle;
				else
					return CostMedian < CostAverage && CostMedian < CostMiddle ? Median : Average;
				
			}

			float BruteForce(std::vector<KDTriangle> Triangles, int Axis, KDBoundingBox Box) {
				
				float BestSolution = -1.f, BestRatio = -1.;

				std::vector<KDTriangleLongestAxis> NewTris;
				for (auto Tri : Triangles) {
					NewTris.push_back(KDTriangleLongestAxis(Tri.Vertices[0][Axis], Tri.Vertices[1][Axis], Tri.Vertices[2][Axis]));
				}

				std::sort(NewTris.begin(), NewTris.end());


				int replace = 0; 

				for (int Tri = 1; Tri < Triangles.size(); Tri++) {
					if (Tri == 1) {
						//this is the best solution 
						BestRatio = NewTris[Tri].AvgPoint(); 

						int Left = 1, Right = NewTris.size()-2; 

						

						float AreaLeft = 0., AreaRight = 0.; 

						AreaLeft = (BestRatio - Box.Min[Axis]) / (Box.Max[Axis] - Box.Min[Axis]);
						AreaRight = 1. - AreaLeft; 
							
						BestSolution = (float(Left*Left*Left) / AreaLeft) + (float(Right*Right*Right) / AreaRight);

					}
					else {

						float TempRatio, TempSolution; 

						TempRatio = NewTris[Tri].AvgPoint();

						int Left = Tri, Right = NewTris.size() - (Tri+1);

						float AreaLeft = 0., AreaRight = 0.;

						AreaLeft = (BestRatio - Box.Min[Axis]) / (Box.Max[Axis] - Box.Min[Axis]);
						AreaRight = 1. - AreaLeft;

						TempSolution = (float(Left*Left*Left) / AreaLeft) + (float(Right*Right*Right) / AreaRight);

						if (TempSolution > BestSolution) {
							BestSolution = TempSolution; 
							BestRatio = TempRatio; 
							replace++; 
							//std::cout << "Replaced!\n (" << replace << ' ' << Triangles.size() << ")\n";
						}

					}

				}

				return BestRatio; 

			}

			const unsigned int SplitPoint = 3; 

			KDNode * ConstructKDTreeFromModel(std::vector<KDTriangle> Tris, int depth)	{
				KDNode * Node = new KDNode();
				Node->Left = nullptr;
				Node->Right = nullptr;
				Node->BoundingBox = KDBoundingBox();
				Node->IsLeaf = false;

				if (Tris.size() == 0)
					return Node;

				if (Tris.size() <= SplitPoint) {
					Node->Triangles = Tris;

					for (const auto & Triangle : Tris) {
						Node->BoundingBox.ResizeToFit(Triangle);
					}
					Node->Left = new KDNode();
					Node->Right = new KDNode();
					Node->Left->Triangles = std::vector<KDTriangle>(0);
					Node->Right->Triangles = std::vector<KDTriangle>(0);
					Node->IsLeaf = true;

					

					return Node;
				}

				for (const auto & Triangle : Tris) {
					Node->BoundingBox.ResizeToFit(Triangle);
				}

				

				int Axis = Node->BoundingBox.LongestAxis();

				float MidPointAverage = Average(Tris, Axis); 
				float MidPointBruteForce = BruteForce(Tris, Axis, Node->BoundingBox); 
				float MidPointMiddle = Middle(Axis, Node->BoundingBox); 
				float MidPointMedian = Median(Tris, Axis); 

				float Solution = ChooseSolution(MidPointMedian, MidPointAverage, MidPointBruteForce, Tris, Node->BoundingBox, Axis);

				//std::cout << MidPointAverage << ' ' << MidPointBruteForce << '\n'; 


				std::vector<KDTriangle> RightTriangles, LeftTriangles;

				for (int i = 0; i < Tris.size(); i++)
					Solution >= Tris[i].Midpoint()[Axis] ? RightTriangles.push_back(Tris[i]) : LeftTriangles.push_back(Tris[i]);





				if (RightTriangles.size() == 0 && LeftTriangles.size() > 0) { 

					//steal the 10 (or 1/2, depends on whats the smallest) right most triangles, essentially creating a leaf node next time


					std::vector<KDTriangleLongestAxisWithIndicie> NewTris; 

					for (int i = 0; i < Tris.size(); i++) {
						NewTris.push_back(KDTriangleLongestAxisWithIndicie(Tris[i].Vertices[0][Axis], Tris[i].Vertices[1][Axis], Tris[i].Vertices[2][Axis], i)); 
					}

					std::sort(NewTris.begin(), NewTris.end()); 

					//as this is sorted from low-high, start at the end and build up 

					std::vector<int> Adresses; 

					for (int i = NewTris.size() - 1; i >= std::max(NewTris.size() / 2, NewTris.size() - SplitPoint); i--) {
						RightTriangles.push_back(LeftTriangles[NewTris[i].Index]); 
						Adresses.push_back(NewTris[i].Index); 
					}

					std::sort(Adresses.begin(), Adresses.end(), std::greater<int>());

					for (auto Adress : Adresses) {
						LeftTriangles.erase(LeftTriangles.begin() + Adress); 
					}


					NewTris.clear(); 
					Adresses.clear(); 

				}
				if (LeftTriangles.size() == 0 && RightTriangles.size() > 0) { 


					std::vector<KDTriangleLongestAxisWithIndicie> NewTris;

					for (int i = 0; i < Tris.size(); i++) {
						NewTris.push_back(KDTriangleLongestAxisWithIndicie(Tris[i].Vertices[0][Axis], Tris[i].Vertices[1][Axis], Tris[i].Vertices[2][Axis], i));
					}

					std::sort(NewTris.begin(), NewTris.end());

					//as this is sorted from low-high, start at the end and build up 

					std::vector<int> Adresses;

					for (int i = 0; i < std::min(NewTris.size()/2,size_t(SplitPoint)); i++) {
						LeftTriangles.push_back(RightTriangles[NewTris[i].Index]);
						Adresses.push_back(NewTris[i].Index);
					}

					std::sort(Adresses.begin(), Adresses.end(), std::greater<int>());

					for (auto Adress : Adresses) {
						RightTriangles.erase(RightTriangles.begin() + Adress);
					}

					NewTris.clear();
					Adresses.clear();

				}
			

				

				
				Node->Left = ConstructKDTreeFromModel(LeftTriangles, depth + 1);
				Node->Right = ConstructKDTreeFromModel(RightTriangles, depth + 1);
					
				return Node;

			}




			void SetPixelData(BSTImageData & Data, KDNode * Node, unsigned int & Offset, bool Right = false, unsigned int RightParent = 0) {



				if (Node->IsLeaf) {

					Vector4f Pixel1 = Vector4f(Node->BoundingBox.Min, Right ? static_cast<float>(RightParent) : -1.f);
					Vector4f Pixel2 = Vector4f(Node->BoundingBox.Max, -static_cast<float>(Node->Triangles.size()));

					Data.ImageData[Offset] = Pixel1;
					Data.ImageData[Offset + 1] = Pixel2;

					Offset += 2;


					for (int i = 0; i < Node->Triangles.size(); i += 4) {

						Data.ImageData[Offset] = Vector4f(-1.f);

						for (int j = i; j < std::min(static_cast<int>(Node->Triangles.size()), i + 4); j++)
							Data.ImageData[Offset][j - i] = static_cast<float>(Node->Triangles[j].StartIndicie);
						Offset++;
					}

				}
				else {

					float Pixel1A = Right ? static_cast<float>(RightParent) : -1.f;
					float Pixel2A = 1.f; //what would this be required for? 

					Vector4f Pixel1 = Vector4f(Node->BoundingBox.Min, Pixel1A);
					Vector4f Pixel2 = Vector4f(Node->BoundingBox.Max, Pixel2A);

					Data.ImageData[Offset] = Pixel1;
					Data.ImageData[Offset + 1] = Pixel2;



					unsigned int Parent = Offset;

					Offset += 2;

					SetPixelData(Data, Node->Left, Offset);
					Data.ImageData[Parent + 1].a = static_cast<float>(Offset);


					SetPixelData(Data, Node->Right, Offset, true, Parent);

				}

			}



			BSTImageData StoreKDTreeInImage(KDNode * Node, unsigned int Width, unsigned int Height)
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