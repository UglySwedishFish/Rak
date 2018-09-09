#version 330
in vec2 TexCoord; 
out vec4 Color; 

const int COLORWHEEL = 0; 
const int SLIDER = 1; 
const int BUTTON = 2; 
const int CHECKBOX = 3; 
const int BACKGROUNDFLAT = 4; 
const int BACKGROUNDSQUIRCLE = 5; 

uniform int Type; 

//THIS WILL ALLOW FOR DIFFERENT THEMES LATER ON, NOT IMPLEMENTED YET 

uniform vec3 BackGroundColor; 
uniform vec3 EdgeColor; 
uniform vec3 FillColor; 

uniform float TypeFloat; 
uniform int TypeInt; 
uniform bool TypeBool; 
uniform vec2 TypeVec2; 
uniform vec3 TypeVec3; 
uniform vec2 ScreenSize; 

uniform vec2 Size; 
uniform vec2 Position; 

uniform sampler2D BackGroundPathTracing;



vec3 HSBToRGB( vec3 HSB ){
    vec3 RGB = clamp(abs(mod(HSB.x * 6.0 + vec3(0.0,4.0,2.0),6.0)-3.0)-1.0,0.0,1.0);
    RGB = RGB * RGB * (3.0 -2.0 * RGB);
    return HSB.z * mix(vec3(1.0),RGB,(HSB.y < .025 ? 0. : HSB.y));
}
const float TAU = 6.28318530718; 

void main() {

	vec3 ActualBackGround = mix(BackGroundColor,textureLod(BackGroundPathTracing, gl_FragCoord.xy/ScreenSize, 6.f).xyz, 0.75); 
	
	

	if(Type == COLORWHEEL) {
		vec2 ToCenterVector = vec2(0.5) - TexCoord;
		float Angle = atan(ToCenterVector.y,ToCenterVector.x);
		float Distance = length(ToCenterVector) * 3.0;

		vec3 ColorWheel = HSBToRGB(vec3((Angle / TAU) + 0.5,Distance,1.0))*TypeVec3.z; 

		Color = mix(vec4(ColorWheel,1.) ,vec4(ColorWheel,0.),min(pow(Distance,10.)+.02,1.));

		//now draw a second circle 

		float DistanceSmallCircle = distance(TypeVec3.xy, TexCoord); 


		ToCenterVector = vec2(0.5) - TypeVec3.xy;
		Angle = atan(ToCenterVector.y,ToCenterVector.x);
		Distance = length(ToCenterVector) * 3.0;

		vec3 CircleColor = DistanceSmallCircle > 0.006 ? vec3(1.-TypeVec3.z) : vec3(TypeVec3.z);  

		Color = DistanceSmallCircle < 0.012 ? vec4(CircleColor,1.) : Color; 

		//draw another circle 

		float DistancePreviewCircle = distance(vec2(0.08, 0.5), TexCoord); 

		vec3 ColorPreview = HSBToRGB(vec3((Angle / TAU) + 0.5,Distance,1.0));

		if(DistancePreviewCircle < 0.08) 
		Color = mix(vec4(ColorPreview*TypeVec3.z,1.), vec4(ColorPreview*TypeVec3.z,0.), pow(min(DistancePreviewCircle*20.,1.),5.)); 

		//draw a squircle 

		vec2 TexCoordSquircle = vec2((((TexCoord.x - .85) * 13.) * 2 - 1.),(((TexCoord.y - 0.1667) * 1.5) * 2 - 1.)); 

		float DistanceSquircle = pow(abs(TexCoordSquircle.x),4.) + pow(abs(TexCoordSquircle.y),8.);
		if(DistanceSquircle < 1.) {
			Color = vec4(ColorPreview*(TexCoordSquircle.y*.5+.5),1.); 
			Color = mix(Color, vec4(Color.xyz,0.), pow(DistanceSquircle,5.)); 
		}

		float DistanceFinalCircle = distance(vec2(0.888, mix(0.2,0.8,TypeVec3.z)), TexCoord); 
		if(DistanceFinalCircle < 0.012) 
		Color = vec4(DistanceFinalCircle > 0.006 ? vec3(1.-TypeVec3.z) : vec3(TypeVec3.z),1.);  

		if(Color.a < 0.01) 
		discard; 

		Color.xyz = mix(ActualBackGround, Color.xyz, Color.a); 
		Color.a = 1.0; 

		

	}
	else if(Type == SLIDER) {
		
		float Distance = pow(abs(TexCoord.x * 2 - 1.),4.) + pow(abs(TexCoord.y * 2 - 1.),4.);

		vec3 SliderColor = mix(mix(vec3(0.), vec3(1.), TexCoord.x),ActualBackGround,0.3); 

		if(Distance < 1.) {
			Color = mix(vec4(SliderColor, 1.), vec4(SliderColor,0.), pow(Distance, 10.0)); 
		}
		else {
			Color = vec4(0.); 
		}

		float DistanceFinalCircle = distance(vec2(TypeFloat, 0.25 * (Size.y / Size.x)), TexCoord * vec2(1., (.5 * Size.y / Size.x))); 

		if(DistanceFinalCircle < 0.012) 
		Color = vec4(DistanceFinalCircle > 0.006 ? vec3(1.) : vec3(0.),1.);  

		if(Color.a < 0.01) 
		discard; 

		Color.xyz = mix(ActualBackGround, Color.xyz, Color.a); 
		Color.a = 1.0; 

	}
	else if(Type == BUTTON) {
		
	}
	else if(Type == CHECKBOX) {
		
	}
	else if(Type == BACKGROUNDFLAT) {
		Color = vec4(ActualBackGround, 1.); 
	}
	else {
		
		

		float Distance = pow(abs(TexCoord.x * 2 - 1.),4.) + pow(abs(TexCoord.y * 2 - 1.),4.);

		if(Distance < 1.) {
			Color = mix(vec4(ActualBackGround, 1.), vec4(ActualBackGround,0.), pow(Distance, 10.0)); 
		}
		else {
			Color = vec4(0.); 
		}
	}
}