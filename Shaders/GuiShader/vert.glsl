#version 330
layout (location = 0) in vec3 Vert; 
layout (location = 1) in vec2 Texc;
out vec2 TexCoord;  

uniform vec2 Size; 
uniform vec2 Position; 

void main(void) {
TexCoord = Texc; 
gl_Position = vec4((Vert * vec3(Size,1.)) + vec3(Position,0.),1.0); 
}