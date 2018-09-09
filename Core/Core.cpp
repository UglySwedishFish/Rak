#include "Core.h"
#include <Dependencies\DependenciesSFML.h> 
#include <iostream> 


template<typename T>
T Rak::Core::Interpolate(T A, T B, T X)
{
	T FT = x * static_cast<T>(3.1415);
}





template<typename T>
T Rak::Core::Lerp(T A, T B, T F)
{
	return A + F * (B - A); 
}

Matrix4f Rak::Core::ViewMatrix(Vector3f Position, Vector3f Rotation)
{
	Matrix4f Temp;
	Temp = glm::rotate(Temp, glm::radians(Rotation.x), { 1, 0, 0 });
	Temp = glm::rotate(Temp, glm::radians(Rotation.y), { 0, 1, 0 });
	Temp = glm::rotate(Temp, glm::radians(Rotation.z), { 0, 0, 1 });

	Temp = glm::translate(Temp, Vector3f(-Position.x, -Position.y, -Position.z));

	return Temp;
}

Matrix4f Rak::Core::ModelMatrix(Vector3f Position, Vector3f Rotation)
{
	return Matrix4f();
}

Matrix4f Rak::Core::ShadowOrthoMatrix(float edge, float znear, float zfar)
{
	return glm::ortho(-edge, edge, -edge, edge, znear, zfar);
}
