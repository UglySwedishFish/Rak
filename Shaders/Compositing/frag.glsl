#version 330
in vec2 TexCoord; 
out vec3 Color; 

uniform sampler2D Lighting; 
uniform sampler2D Albedo; 
uniform samplerCube Enviroment; 
uniform mat4 ViewMatrix; 
uniform mat4 ProjectionMatrix; 

void main() {	

	vec4 AlbedoSample = texture(Albedo, TexCoord); 

	if(AlbedoSample.a < -.95) {
		vec4 Direction = inverse(ProjectionMatrix * mat4(mat3(ViewMatrix))) * vec4(TexCoord * 2. - 1., 1.f, 1.f);
		Direction.xyz /= Direction.w; 
		Color = texture(Enviroment, Direction.xyz).xyz;
	}
	else {
		Color = texture(Lighting, TexCoord).xyz + AlbedoSample.xyz * AlbedoSample.aaa;
	}
	Color = pow(Color,vec3(.454545)); //.454545 is for gamma correction		
}