#include "meshselector.h"
#include <iostream>



using std::max;
using std::min;
using glm::dot;
using glm::cross;

bool RayTriangleIntersection(Vector3f edge1, Vector3f edge2, Vector3f edge3, Rak::Rendering::Mesh::Ray ray, float & Traversal, float ClosestDistance) {

	Vector3f e0 = edge2 - edge1;
	Vector3f e1 = edge3 - edge1;

	Vector3f h = cross(ray.Direction, e1);
	float a = dot(e0, h);

	if (a > -0.0000001 && a < 0.0000001)
		return false;

	float f = 1.0 / a;

	Vector3f s = ray.Origin - edge1;
	float u = f * dot(s, h);



	Vector3f q = cross(s, e0);
	float v = f * dot(ray.Direction, q);

	Traversal = f * dot(e1, q);

	if (Traversal < 0.01)
		return false;
	if (u < 0.0 || u > 1.0)
		return false;
	if (v < 0.0 || u + v > 1.0)
		return false;

	return (Traversal > 0.0) && (Traversal < ClosestDistance || ClosestDistance < 0.);

}


void Rak::Rendering::Mesh::Select(Window & Window, const Control::Camera & Camera,const std::vector<Model>& Models, int & Model, int & Material)
{
	float ClosestDistance = -1.f; 
	Ray Ray; 

	Vector2f MousePos = (Vector2f(sf::Mouse::getPosition(*Window.GetRawWindow()).x, sf::Mouse::getPosition(*Window.GetRawWindow()).y) / Vector2f(Window.GetResolution()));

	Ray.Direction = glm::normalize(Vector3f(glm::inverse(Camera.Project * Matrix4f(Matrix3f(Camera.View))) * Vector4f(MousePos.x * 2. -1., (1.-MousePos.y) * 2.f - 1.f, 1.f, 1.f)));
	Ray.Origin = Camera.Position; 

	for (int CurrentModel = 0; CurrentModel < Models.size(); CurrentModel++) {
		for (int Triangle = 0; Triangle < Models[CurrentModel].ModelData.Vertices.size(); Triangle+=3) {
			float TemporaryDistance = 0.f; 
			if (RayTriangleIntersection(Models[CurrentModel].ModelData.Vertices[Triangle], Models[CurrentModel].ModelData.Vertices[Triangle + 1], Models[CurrentModel].ModelData.Vertices[Triangle + 2], Ray, TemporaryDistance, ClosestDistance)) {
				Material = floor(Models[CurrentModel].ModelData.TextureCoordinates[Triangle].z * 100.f + .1f);
				Model = CurrentModel; 
			}
		}
	}
}
