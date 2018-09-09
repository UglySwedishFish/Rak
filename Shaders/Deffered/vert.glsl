#version 330
layout(location=0) in vec3 Pos; 
layout(location=1) in vec3 TexCoords; 
layout(location=2) in vec3 Normals; 
layout(location=3) in vec3 Tangents; 

uniform mat4 ViewMatrix; 
uniform mat4 ModelMatrix; 
uniform mat4 ProjectionMatrix;

out VertexValues {
vec3 Position;
vec3 TexCoord;  
vec3 Normal; 
vec3 Tangent;

} VS_OUT; 

void main(void) {	
	gl_Position = ProjectionMatrix * ViewMatrix * vec4(Pos,1.0); 
	VS_OUT.Position = Pos;
	VS_OUT.Normal = Normals; 
	VS_OUT.TexCoord = TexCoords; 
	VS_OUT.Tangent = Tangents;
}