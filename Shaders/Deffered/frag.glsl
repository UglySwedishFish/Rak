#version 330

layout(location = 0) out vec4 Color; 
layout(location = 1) out vec4 NormalRefractive;
layout(location = 2) out vec4 PositionSpecular;  

in VertexValues {
vec3 Position;
vec3 TexCoord;  
vec3 Normal; 
vec3 Tangent;

} FS_IN; 

vec3 CalcNormalMappedNormal(sampler2DArray Normalmap,vec3 Normal,vec3 TexCoord,float intensity,mat3 TBN)  { //for normal maps 
    return mix(Normal,(TBN * (texture(Normalmap,TexCoord).rgb * 2. -1.)),intensity);
}

uniform mat4 ViewMatrix; 
uniform mat4 ModelMatrix; 
uniform mat4 ProjectionMatrix;

uniform sampler2DArray Albedo; 
uniform sampler2DArray DRM; 
uniform sampler2DArray Normal; 

uniform sampler1D Materials; 

void main() {
	
	int Index = int(floor(FS_IN.TexCoord.z * 100.f + .1f)); 
	vec4 RawMaterial = texelFetch(Materials, Index, 0); 
	int Material = int(floor(RawMaterial.a * 128.f + .1f)); 

	vec3 TextureCoordinate = vec3(FS_IN.TexCoord.xy,float(Material)); 
	
	vec3 Norm = normalize(FS_IN.Normal);
    vec3 Tang = normalize(FS_IN.Tangent);
    vec3 Tangent = normalize(Tang - dot(Tang, Norm) * Norm);
    vec3 Bitangent = normalize(cross(Tang, Norm));
	mat3 TBN = mat3(Tangent,Bitangent,Norm); 
		
	Color.xyz = textureLod(Albedo,TextureCoordinate, 0.).xyz;  	
	PositionSpecular.xyz = FS_IN.Position;
	NormalRefractive.xyz = normalize(CalcNormalMappedNormal(Normal, Norm, TextureCoordinate, 0.25, TBN)); 

	vec3 DRM = textureLod(DRM, TextureCoordinate, 0.).xyz; 

	Color.a = 1.0; 
	NormalRefractive.a = DRM.y; 
	PositionSpecular.a = DRM.z; 

}