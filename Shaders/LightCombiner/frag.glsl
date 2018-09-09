#version 330 

in vec2 TexCoord; 
out vec4 Final; 

uniform sampler2D RawTrace; 
uniform sampler2D HalfResPosition; 
uniform sampler2D PositionMetallic; 

uniform vec3 CameraPosition; 

float SquaredDistance(vec3 a, vec3 b) {
	return dot(a-b, a-b); 
}

const vec2 Offsets[8] = vec2[8](vec2(1,0),vec2(-1,0), vec2(0,1), vec2(0,-1), vec2(1, 1), vec2(1, -1), vec2(-1, 1), vec2(-1,-1)); 
//stores the neighboring pixels

void main() {
	vec2 Offset = vec2(0.); //offset to the best pixel (assumed to be 0 by default) 
	ivec2 ImageSize = textureSize(HalfResPosition, 0); 
	vec2 TexelSize = 1. / vec2(ImageSize); 

	vec3 FullResPosition = texture(PositionMetallic, TexCoord).xyz; 

	vec3 CurrentHalfResPosition = texture(HalfResPosition, TexCoord).xyz; 

	float CurrentDistanceSquared = SquaredDistance(CurrentHalfResPosition, FullResPosition); 

	for(int SorroundingPixels = 0; SorroundingPixels < 8; SorroundingPixels++) {
		//search through neighboring pixels

		CurrentHalfResPosition = texture(HalfResPosition, TexCoord + (Offsets[SorroundingPixels] * TexelSize)).xyz;

		float DistanceSquared = SquaredDistance(CurrentHalfResPosition, FullResPosition); 

		Offset = CurrentDistanceSquared > DistanceSquared ? vec2(Offsets[SorroundingPixels]) * TexelSize : Offset; 
		//is there a better pixel? use that one instead
		CurrentDistanceSquared = CurrentDistanceSquared > DistanceSquared ? DistanceSquared : CurrentDistanceSquared; 
	}
	
	Final = texture(RawTrace, TexCoord + Offset); //upscaled version 
}