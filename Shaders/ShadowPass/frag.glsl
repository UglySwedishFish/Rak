#version 330
out vec3 Color; 

//oooooh exciting (not really) 

void main() {
	Color = vec3(gl_FragCoord.z); 
}