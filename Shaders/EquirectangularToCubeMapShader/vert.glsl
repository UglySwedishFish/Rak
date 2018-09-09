#version 330
layout (location = 0) in vec3 Vert; 
out vec3 Vertice; 
uniform mat4 ProjectionMatrix; 
uniform mat4 ViewMatrix; 
void main(void) {
	Vertice = Vert; 
	gl_Position = (ProjectionMatrix * mat4(mat3(ViewMatrix)) * vec4(Vert,1.0)).xyww; 
}