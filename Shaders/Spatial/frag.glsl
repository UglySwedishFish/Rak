#version 330
in vec2 TexCoord; 
out vec4 OutPut; 

uniform sampler2D RawInput; 
uniform sampler2D NormalRoughness; 
uniform sampler2D PositionMetallic; 
uniform bool Vertical; 
uniform bool First; 

uniform mat4 ViewMatrix; 
uniform mat4 ProjectionMatrix; 

void main() {
	
	//we should first figure out how much the spatial filter should be used
	//so that we dont overblur anything 

	ivec2 TextureSize = textureSize(RawInput, 0); 
	vec2 TexelSize = 1. / vec2(TextureSize); 

	vec4 CurrentPixel = texture(RawInput, TexCoord);
	float Usage = 1.0; 
	
	if(CurrentPixel.a < 0.) {
		//this is a specular ray, thus we must be very careful
		//so that we dont overblur it 
		float Usage = mix(.05, .25, texture(NormalRoughness, TexCoord).a); 
		//blend between usage factors 0.1 and 0.5 based on the roughness of the material
	}

	//now, convert the usage number into something we can actually use. 

	vec4 CurrentWorldPosition = texture(PositionMetallic, TexCoord);

	vec3 WorldPosPositive = texture(PositionMetallic, TexCoord + vec2(!Vertical ? TexelSize.x : 0.0, Vertical ? TexelSize.y : 0.0)).xyz;
	vec3 WorldPosNegative = texture(PositionMetallic, TexCoord - vec2(!Vertical ? TexelSize.x : 0.0, Vertical ? TexelSize.y : 0.0)).xyz;

	vec3 RayDirectionPositive = normalize(WorldPosPositive - CurrentWorldPosition.xyz);
	vec3 RayDirectionNegative = normalize(WorldPosPositive - CurrentWorldPosition.xyz);

	vec4 NewWorldPositionPositive = vec4(CurrentWorldPosition.xyz + RayDirectionPositive*0.5, 1.);
	vec4 NewWorldPositionNegative = vec4(CurrentWorldPosition.xyz + RayDirectionNegative*0.5, 1.);

	vec4 NDCPositive = ProjectionMatrix * ViewMatrix * NewWorldPositionPositive;
	vec2 TCPositive = (NDCPositive.xy / NDCPositive.w) * 0.5 + 0.5;

	vec4 NDCNegative = ProjectionMatrix * ViewMatrix * NewWorldPositionPositive;
	vec2 TCNegative = (NDCNegative.xy / NDCNegative.w) * 0.5 + 0.5;

	int RangeNegative = min(max(Vertical ? int(floor(abs(TCNegative.y - TexCoord.y) * TextureSize.y)) : int(floor(abs(TCNegative.x - TexCoord.x) * TextureSize.x)), int(ceil(mix(2., 4., Usage)))), int(ceil(mix(4., 8.0, Usage))));
	int RangePositive = min(max(Vertical ? int(floor(abs(TCPositive.y - TexCoord.y) * TextureSize.y)) : int(floor(abs(TCPositive.x - TexCoord.x) * TextureSize.x)), int(ceil(mix(2., 4., Usage)))), int(ceil(mix(4., 8.0, Usage))));

	float PowerCoef = mix(0.2, 0.1, Usage);

	//run a pretty naive spatial filter 

	vec3 Result = vec3(0.); 
	float PixelAddon = 0.; 
	float Power = 1.0; 

	vec4 PrevWorldPos = CurrentWorldPosition; 

	for (int Pixel = -1; Pixel > RangeNegative; Pixel--) {

		Power -= PowerCoef;

		vec2 Addon = vec2(!Vertical ? TexelSize.x * Pixel : 0., Vertical ? TexelSize.y * Pixel : 0.);
		vec2 NC = TexCoord + Addon;
		vec4 ThisWorldPos = texture(PositionMetallic, NC);

		if (abs(ThisWorldPos.x - PrevWorldPos.x) > 0.1 || abs(ThisWorldPos.y - PrevWorldPos.y) > 0.1 || abs(ThisWorldPos.z - PrevWorldPos.z) > 0.1 || abs(ThisWorldPos.w - PrevWorldPos.w) > 0.1)
			break;

		PrevWorldPos = ThisWorldPos;
		Result += texture(RawInput, NC).rgb * Power;
		PixelAddon += 1.0 * Power;

	}

	for (int Pixel = 1; Pixel < RangePositive; Pixel++) {
		Power -= PowerCoef;

		vec2 Addon = vec2(!Vertical ? TexelSize.x * Pixel : 0., Vertical ? TexelSize.y * Pixel : 0.);
		vec2 NC = TexCoord + Addon;
		vec4 ThisWorldPos = texture(PositionMetallic, NC);

		if (abs(ThisWorldPos.x - PrevWorldPos.x) > 0.1 || abs(ThisWorldPos.y - PrevWorldPos.y) > 0.1 || abs(ThisWorldPos.z - PrevWorldPos.z) > 0.1 || abs(ThisWorldPos.w - PrevWorldPos.w) > 0.1)
			break;

		PrevWorldPos = ThisWorldPos;
		Result += texture(RawInput, NC).rgb * Power;
		PixelAddon += 1.0 * Power;
	}

	Result = PixelAddon < 0.1 ? CurrentPixel.xyz : Result / PixelAddon;

	OutPut = vec4(Result, CurrentPixel.w); 

}