#version 330
in vec2 TexCoord; 

layout(location = 0) out vec3 Sampling; 
layout(location = 1) out vec3 Result; 

uniform sampler2D PreviousSampling; 
uniform sampler2D PreviousWorldPos;
uniform sampler2D PreviousTemporal;
uniform sampler2D PreviousRawTrace; 
uniform sampler2D Trace; 
uniform sampler2D WorldPosMetallic; 
uniform sampler2D Albedo;

uniform bool First; 
uniform int CheckerBoard; 

uniform mat4 ViewMatrix; 
uniform mat4 PreviousViewMatrix; 
uniform mat4 ProjectionMatrix; 



void main() {


	
	bool IsCurrentPixelTwoBounce = (int(gl_FragCoord.x + gl_FragCoord.y) % 2 == CheckerBoard); 
	
	vec4 Alb = texture(Albedo, TexCoord); 
	vec4 WorldPos = texture(WorldPosMetallic, TexCoord); 

	vec3 SpecularColor = mix(vec3(1.), Alb.xyz, WorldPos.w); 
	vec3 DiffuseColor = mix(Alb.xyz, vec3(1.), WorldPos.w); 

	vec4 CurrentTrace = texture(Trace, TexCoord); 
	CurrentTrace.xyz = CurrentTrace.xyz * (CurrentTrace.w > 0. ? DiffuseColor : SpecularColor); 

	CurrentTrace.xyz = IsCurrentPixelTwoBounce ? CurrentTrace.xyz : texture(PreviousRawTrace, TexCoord).xyz; 
	

	if (!First) {
		
		vec3 PreviousTemporalSampling = texture(PreviousSampling, TexCoord).rgb;

		int Frame = int(floor(PreviousTemporalSampling.r*100.0 + .1)) + 1;

		//compute old texture coordinate for current world position

		vec4 NDC = ProjectionMatrix * PreviousViewMatrix * vec4(WorldPos.xyz, 1.0);

		vec2 TC = NDC.xy / NDC.w;

		//check if said pixel is in the screen or not
		
		if (abs(TC.x) < 1. && abs(TC.y) < 1.) {

			TC = TC * .5 + .5;

			vec4 PrevWorldPos = texture(PreviousWorldPos, TC);

			vec4 Diff = abs(PrevWorldPos - WorldPos);

			//check if said pixel is "good" enough to be used for temporal filtering

			if (Diff.x < 0.001 && Diff.y < 0.001 && Diff.z < 0.001 && PrevWorldPos.w > -0.1) {

				//check if frame needs to be reset or not (for start we're not going to be doing that) 


				//compute current frame 

				float Temporal = min(float(Frame) / float(Frame + 1), 0.98); //0.96666 is used so that temporal filtering
				//doesnt take ages to reset
				//this means that if I for instance change a lights direction and/or position the temporal filtering doesnt
				//completely ruin the realism 

				Result = mix(CurrentTrace.xyz, texture(PreviousTemporal, TC).rgb, Temporal); //mix between this and previous frame

				Sampling = vec3(float(min(Frame, 1000)) / 100.0); //store sampling

				return;

			}
		}
		
	}

	Result = CurrentTrace.xyz;
	Sampling = vec3(0.);
}