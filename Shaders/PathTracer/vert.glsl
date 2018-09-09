#version 330
layout (location = 0) in vec3 Vert; 
layout (location = 1) in vec2 Texc;
out vec2 TexCoord;  
void main(void) {
TexCoord = Texc; 
gl_Position = vec4(Vert,1.0); 
}