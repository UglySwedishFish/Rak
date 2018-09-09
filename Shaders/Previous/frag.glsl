#version 330
in vec2 TexCoord; 

layout(location = 0) out vec4 OutSamples; 
layout(location = 1) out vec4 OutPosition; 
layout(location = 2) out vec4 OutRawTrace; 
layout(location = 3) out vec4 OutTemporalTrace;  
layout(location = 4) out vec4 PreviousHit; 
layout(location = 5) out vec4 PreviousDirection; 
layout(location = 6) out vec4 PreviousIndirect; 
layout(location = 7) out vec4 PreviousNormal; 

uniform sampler2D Samples; 
uniform sampler2D Position; 
uniform sampler2D RawTrace; 
uniform sampler2D TemporalTrace; 
uniform sampler2D Albedo; 
uniform sampler2D Hit; 
uniform sampler2D Direction; 
uniform sampler2D Indirect; 
uniform sampler2D Normal; 

uniform mat4 ViewMatrix; 

//pretty simple, just stores a ton of previous data

void main() {

	OutSamples = texture(Samples, TexCoord); 
	OutPosition = texture(Position, TexCoord); 
	vec4 Alb = texture(Albedo, TexCoord); 
	OutRawTrace = texture(RawTrace, TexCoord); 
	OutRawTrace.xyz = OutRawTrace.xyz * (OutRawTrace.w < 0. ? mix(vec3(1.), Alb.xyz, OutPosition.w) : mix(Alb.xyz, vec3(1.), OutPosition.w)); 

	OutTemporalTrace = texture(TemporalTrace, TexCoord); 

	ivec2 Res = textureSize(Hit, 0); 
	ivec2 Pos = ivec2(floor(TexCoord * vec2(Res))+.1); 

	PreviousHit = texelFetch(Hit, Pos, 0); 
	PreviousDirection = texelFetch(Direction, Pos, 0); 
	PreviousIndirect = texelFetch(Indirect, Pos, 0); 
	PreviousNormal = texelFetch(Normal, Pos, 0); 
}