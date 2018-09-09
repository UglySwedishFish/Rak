#version 440
#extension GL_ARB_bindless_texture : enable
#extension GL_ARB_gpu_shader5 : enable

in vec2 TexCoord; 

layout(location = 0) out vec4 OutTrace; 
layout(location = 1) out vec4 OutPosition; 
layout(location = 2) out vec4 Hit; 
layout(location = 3) out vec4 Direction; 
layout(location = 4) out vec4 IndirectHit; //this stores the color of the surface we hit, doesnt care about lighting ;) 
layout(location = 5) out vec4 NormalHit;

uniform sampler2D PositionSpecular; 
uniform sampler2D NormalRefractive; 
uniform sampler2D IOREmmisisionRoughness; 
uniform sampler2D PreviousHit; 
uniform sampler2D PreviousDirection; 
uniform sampler2D PreviousIndirectHit; 
uniform sampler2D PreviousNormalHit; 
uniform sampler2D ModelMaterials; 
uniform sampler2DArray LowResTextures; 
uniform sampler2DArray ModelData[14]; 
uniform sampler2DShadow DirectionalShadowMap;
uniform samplerCube Enviroment; 
uniform mat4 DirectionalProjectionMatrix; 
uniform mat4 DirectionalViewMatrix;
uniform vec3 SunDirection; 




//TODO: Add comments (proper ones, not like this one), Split functions into files to make code more readable. Remove KD-Tree traversing



uniform int VerticeCount[14]; 
uniform ivec2 Resolution[14]; 

uniform int TotalModels; 

uniform mat4 ProjectionMatrix; 
uniform mat4 ViewMatrix; 
uniform vec3 CameraPosition; 

uniform int Frame; 
uniform int CheckerBoard; 

struct Ray {
	vec3 Direction,Origin, OneOverDirection; 
}; 

const float EPSILON = 0.0000001; 

sampler2DArray GetTexture(int Ind, sampler2DArray Array[14]) {
	return Ind == 0 ? Array[0] : 
			Ind == 1 ? Array[1] : 
			Ind == 2 ? Array[2] : 
			Ind == 3 ? Array[3] : 
			Ind == 4 ? Array[4] : 
			Ind == 5 ? Array[5] : 
			Ind == 6 ? Array[6] : 
			Ind == 7 ? Array[7] : 
			Ind == 8 ? Array[8] : 
			Ind == 9 ? Array[9] : 
			Ind == 10 ? Array[10] : 
			Ind == 11 ? Array[11] : 
			Ind == 12 ? Array[12] : Array[13]; 
}

/*
if only life was as simple as 
return Array[Ind]; 
But, as I cry in the night 
not all gpus support dynamic indicies for arrays of sampler2DArrays
and thus, a hack like this had to be implemented
*/


//simple moller-trumbore ray-triangle intersection (from wikipedia) 
bool RayTriangleIntersection(vec3 edge1, vec3 edge2, vec3 edge3, Ray ray, inout vec2 uv, inout float t,float tGlobal) {

	vec3 e0 = edge2 - edge1; 
	vec3 e1 = edge3 - edge1; 

	vec3 h = cross(ray.Direction, e1); 
	float a = dot(e0, h); 

	if(a > -EPSILON && a < EPSILON) 
		return false; 

	float f = 1.0 / a; 

	vec3 s = ray.Origin - edge1; 
	float u = f * dot(s,h); 

	if (u < 0.0 || u > 1.0)
		return false;

	vec3 q = cross(s, e0);
	float v = f * dot(ray.Direction, q);

	if (v < 0.0 || u + v > 1.0)
		return false;
	
	uv = vec2(u,v); 

	t = f * dot(e1, q);

	if (t < 0.01)
		return false;

	return (t > 0.0) && (t < tGlobal || tGlobal < 0.);
	
} 


ivec2 WrapTo2DTexture(int texel, int resolution) {
	float TexelRes = (float(texel)+.1) / float(resolution); 
	int TexelResDivider = int(floor(TexelRes)); 

	return ivec2(TexelResDivider,texel - TexelResDivider*resolution); 
}

vec3 GetTriangleData(int index,int model, int dataWanted) {
	int Y = int(floor((float(index)+.1)/float(Resolution[model].x))); 
	int XSub = Y*Resolution[model].x; 

	ivec2 texel = ivec2(index-XSub, Y); 
	
	return texelFetch(GetTexture(model,ModelData),ivec3(texel, dataWanted),0).rgb; 
}



//https://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms 
//^ great site for multiple implementations and discussion on ray aabb intersection

bool RayBoxIntersection(Ray ray, vec3 Min, vec3 Max,out float t) {

	float t1 = (Min.x - ray.Origin.x)*ray.OneOverDirection.x;
	float t2 = (Max.x - ray.Origin.x)*ray.OneOverDirection.x;
	float t3 = (Min.y - ray.Origin.y)*ray.OneOverDirection.y;
	float t4 = (Max.y - ray.Origin.y)*ray.OneOverDirection.y;
	float t5 = (Min.z - ray.Origin.z)*ray.OneOverDirection.z;
	float t6 = (Max.z - ray.Origin.z)*ray.OneOverDirection.z;

	float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
	float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

	t = tmin; 

	return tmax > 0.f && tmax > tmin;  

}

int ToInt(float Data) {
	return floatBitsToInt(Data); 
}

float GetVec4Color(vec4 Input, int Index) {
		return 
		Index == 0 ? Input.x : 
		Index == 1 ? Input.y : 
		Index == 2 ? Input.z : 
		Input.w; 
}


const int MAX_STACK_SIZE = 100; 


//hopefully a LOT faster 
float RayModelIntersectionBVH(Ray ray, int Mesh, inout int TriangleHit, inout vec2 UV, out float IntersectionCosts) {


	TriangleHit = -1;

	int Stack[MAX_STACK_SIZE];
	int StackID = 0;
	Stack[StackID++] = 0;
	float ClosestDistance = -1.f;
	int CurrentIndex = 0;
	float Div = 1.0 / 100.;

	int Depth = 1;
	float TempTraversal = 0.;

	//what is the ratio between triangles hit and leaves hit? 
	//this is useful for debugging performance
	//the higher number the better 

	int TrianglesHit = 0;
	int LeavesHit = 0;


	while (StackID) {
		StackID--;
		int BoxID = Stack[StackID];
		vec4 Data1 = texelFetch(GetTexture(Mesh, ModelData), ivec3(WrapTo2DTexture(BoxID, Resolution[Mesh].x).yx, 4), 0);
		vec4 Data2 = texelFetch(GetTexture(Mesh, ModelData), ivec3(WrapTo2DTexture(BoxID + 1, Resolution[Mesh].x).yx, 4), 0);

		if (Data2.a < 0.) { //Branch node

							//IntersectionCosts += Div; 
			if (RayBoxIntersection(ray, Data1.xyz, Data2.xyz, TempTraversal) && (TempTraversal < ClosestDistance || ClosestDistance < 0.)) {

				Stack[StackID++] = ToInt(Data1.a);
				Stack[StackID++] = BoxID + 2;

				if (StackID > MAX_STACK_SIZE) {
					return -100.;
				}


				//if(Depth == 8) {
				//	ClosestDistance = TempTraversal; 
				//}



			}

		}
		else {
			//LEAF NODE 

			if (RayBoxIntersection(ray, Data1.xyz, Data2.xyz, TempTraversal) && (TempTraversal < ClosestDistance || ClosestDistance < 0.)) {

				int StartIndex = ToInt(Data1.a);
				int IndexSize = ToInt(Data2.a);


				for (int Index = 0; Index < IndexSize; Index++) {
					vec3 Triangle1, Triangle2, Triangle3;

					Triangle1 = GetTriangleData((StartIndex + Index) * 3, Mesh, 0);
					Triangle2 = GetTriangleData((StartIndex + Index) * 3 + 1, Mesh, 0);
					Triangle3 = GetTriangleData((StartIndex + Index) * 3 + 2, Mesh, 0);
					vec2 tuv;

					IntersectionCosts += Div;
					if (RayTriangleIntersection(Triangle1, Triangle2, Triangle3, ray, tuv, TempTraversal, ClosestDistance)) {
						TrianglesHit++;
						ClosestDistance = TempTraversal;
						TriangleHit = (StartIndex + Index) * 3;
						UV = tuv;


					}
				}
			}
		}
	}

	return ClosestDistance;

}

//modern ray-world intersection algorithm, using a SAH BVH
float RayWorldIntersectionBVH(Ray ray, inout int MeshHit, inout int TriangleHit, inout vec2 UV, out float IntersectionCosts) {
	MeshHit = -1; 
	TriangleHit = -1; 

	float TTemporary, TClosest = -1.f; 
	vec2 TemporaryUV; 
	int TTriangleHit; 

	for(int Mesh = 0; Mesh < TotalModels; Mesh++) {
		TTemporary = RayModelIntersectionBVH(ray, Mesh, TTriangleHit, TemporaryUV, IntersectionCosts); 
		if((TTemporary < TClosest || TClosest < 0.) && TTemporary > 0.) {
			TClosest = TTemporary; 
			MeshHit = Mesh; 
			TriangleHit = TTriangleHit; 
			UV = TemporaryUV; 
		}
	}
	return TClosest; 
}

float seed; 

//2-component rand
vec2 hash2() {
    return fract(sin(vec2(seed+=0.1,seed+=0.1))*vec2(43758.5453123,22578.1459123));
}

//1-component rand
float rand() {
	return fract(sin(seed+=0.1) * 43758.5453123); 
}

//a random hemisphere function
vec3 cosWeightedRandomHemisphereDirection( const vec3 n) {
  	vec2 rv2 = hash2();
    
	vec3  uu = normalize( cross( n, vec3(0.0,1.0,1.0) ) );
	vec3  vv = normalize( cross( uu, n ) );
	
	float ra = sqrt(rv2.y);
	float rx = ra*cos(6.2831*rv2.x); 
	float ry = ra*sin(6.2831*rv2.x);
	float rz = sqrt( 1.0-rv2.y );
	vec3  rr = vec3( rx*uu + ry*vv + rz*n );
    
    return normalize( rr );
}

float Barycentric(vec2 A, vec2 B, vec2 C) {
	return abs(A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y)); 
}


//old, very slow, naive aproach. Only here for debug purposes. 
float RayWorldIntersection(Ray ray,inout int meshHit,inout int TriangleHit,inout vec2 UvCord, float d) {

	

	meshHit=-1; 
	TriangleHit=-1; 
	float tm = -1.f; 
	float t = -1.f; 
	float _t = -1.f; 


	for (int Mesh = 0; Mesh < TotalModels; Mesh++) {

		const int i = 0; 

		for (int Triangle = i; Triangle < VerticeCount[Mesh]; Triangle+=3) {

			vec3 Triangle1 = GetTriangleData(Triangle,Mesh,0); 
			vec3 Triangle2 = GetTriangleData(Triangle+1,Mesh,0); 
			vec3 Triangle3 = GetTriangleData(Triangle+2,Mesh,0); 
			vec2 tuv; 

			if (RayTriangleIntersection(Triangle1,Triangle2,Triangle3, ray,tuv, tm, t)) {
				meshHit = Mesh; 
				TriangleHit = Triangle; 
				t = tm; 
				UvCord = tuv; 
			}
		}
	}
	return t; 
}

//an aproximation to fresnel, with some added roughness for spicier fresnel 
float SchlicksAproximation(vec3 rdir, vec3 norm, float metallic, float roughness) {
	return clamp(metallic + (max((1.-roughness)-metallic, 0.))*pow(1.0 - dot(-rdir,norm),5.0), 0., 1.); //
}

//constants, will in the future be either uniforms or derived from something a bit more interesting (say a function or an HDRI) 
const vec3 SKYBLUE = vec3(0.0,0.5,1.0); 
const vec3 SUNYELLOW = vec3(1.0, .721, 0.075); 


//simple, directional shadows using something similar to stratified possion disk sampling, although not exactly the same
vec3 GetDirectionalShadowColor(vec3 WorldPos, int Samples, float Specular,float Roughness, vec3 Normal, vec3 Incident) { 

	//find where the world position is currently on the shadowmap 

	float DiffuseFactor = max(dot(SunDirection, Normal),0.); 
	float SpecularFactor = pow(dot(-SunDirection, reflect(Incident, Normal)), 5.0 + (1.0-Roughness)*145.); 

	SpecularFactor = clamp(SpecularFactor, 0., 1.); 

	//this is not physically based at all, but eh 
	
	float ActualFactor = mix(DiffuseFactor, SpecularFactor, Specular); 

	//clip space, ndc space, screen space

	vec4 ClipSpace = DirectionalProjectionMatrix * DirectionalViewMatrix * vec4(WorldPos, 1.0); 

	vec3 NDCSpace = ClipSpace.xyz / ClipSpace.w; 

	vec3 ScreenSpace = NDCSpace * .5 + .5; 

	float Factor = 1.0 / float(Samples); 
	
	vec3 Result = vec3(0.); 

	for(int Sample = 0; Sample < Samples; Sample++) {

		vec2 Direction = normalize(vec2(rand(),rand())); 
		//the direction is used to add some extra fake detail to the shadow, and to make it more smooth
		//it does unfortunately also introduce some noise into the actual shadow. 
		//but, this is taken care of by both the temporal and spatial filtering
		//(that also take care of the raw path traced sample) 

		Result += texture(DirectionalShadowMap, vec3(ScreenSpace.xy + Direction * .002, ScreenSpace.z - 0.0003)).xxx; 

	}

	return Result * Factor * SUNYELLOW * 3. * ActualFactor; 
}

vec3 GetSkyColor(vec3 Direction) {
	return texture(Enviroment, Direction).xyz * 3.; 
}

void main() {


	bool IsSecondaryRay = (int(gl_FragCoord.x + gl_FragCoord.y) % 2 == CheckerBoard); //a checkerboarded aproach to 
	//multi-bounce path tracing. Means that only one trace per 1/4th pixel occurs
	//making it very fast. 


	//get the seed, so that sampling is different between pixels and frames
	seed = ((TexCoord.x * TexCoord.y)*100.0) + float(Frame); 

	//the deffered data
	vec4 PositionRaw = texture(PositionSpecular, TexCoord); 
	vec4 NormalRaw = texture(NormalRefractive, TexCoord); 
	

	//our ray
	Ray RAY; 

	//vec3 RDir = normalize(vec3(inverse(ProjectionMatrix * mat4(mat3(ViewMatrix))) * vec4(TexCoord * 2. - 1., 1.f, 1.f))); 
	//incident ray direction, only for testing purposes

	vec3 Diffuse, Specular; 

	float Metallic = PositionRaw.w; //the metalness of a surface is held in the alpha component of the world position 
	//from the deffered frame buffer

	vec3 Incident; //the incident ray direction, useful to get the specular ray
	
	vec4 Result = vec4(0.); //the result

	float FresnelFactor; //the true reflectivity of the surface, using an aproximation to fresnel

	float AO = 1.f; 

	if(!IsSecondaryRay) {

		RAY.Origin = PositionRaw.xyz; 

		Incident = normalize(PositionRaw.xyz - CameraPosition); 

		Diffuse = normalize(cosWeightedRandomHemisphereDirection(NormalRaw.xyz)); 
		Specular = reflect(Incident, normalize(mix(NormalRaw.xyz, Diffuse, NormalRaw.w * .5))); 

		FresnelFactor = SchlicksAproximation(Incident, NormalRaw.xyz, Metallic, NormalRaw.w);  

		Result.xyz += GetDirectionalShadowColor(PositionRaw.xyz, 4, FresnelFactor, NormalRaw.w, NormalRaw.xyz, Incident); 

	}
	else {

		vec4 NewOrigin = texture(PreviousHit, TexCoord); 
		vec4 IncidentRay = texture(PreviousDirection, TexCoord); 
		vec4 NewNormal = texture(PreviousNormalHit, TexCoord); 
		Incident = IncidentRay.xyz; 

		RAY.Origin = NewOrigin.xyz; 
		Diffuse = normalize(cosWeightedRandomHemisphereDirection(NewNormal.xyz)); 
		Specular = reflect(IncidentRay.xyz, normalize(mix(NewNormal.xyz, Diffuse, NewNormal.w * .5))); 
		NormalRaw = NewNormal; 
		Metallic = NewOrigin.w; 

		FresnelFactor = SchlicksAproximation(Incident, NormalRaw.xyz, Metallic, NormalRaw.w);  

		if(NewOrigin.a < 0.) {
			OutTrace = vec4(GetSkyColor(IncidentRay.xyz), 1.); 
			OutPosition = PositionRaw; 
			return; 
		}

	}

	
	

	bool IsDiffuseRay = rand() > FresnelFactor; 
	//if a random value between 0-1 is above the reflectivity value of the pixel, the ray is diffuse

	RAY.Direction = IsDiffuseRay ? Diffuse : Specular; 
	RAY.OneOverDirection = vec3(1.) / RAY.Direction; //an optimization for the ray-aabb intersection
	
	vec3 Indirect = vec3(1.); //the rays color (assumed to be white at first)

	int Mesh, Triangle; //the mesh and triangles hit
	vec2 UV; //the UV of the triangle (not the actual UV, but can be used to get uv via barycentric interpolation)
	float IntersectionCost; //useful for debug purposes, if used with the incident ray
	float Traversal = RayWorldIntersectionBVH(RAY, Mesh, Triangle, UV, IntersectionCost); //the intersection
	
	if(Triangle != -1) { //if we did indeed hit a triangle
		
		AO = max(min(pow(Traversal * .5,2.),1.),.125); 

		vec2 StoredUVC[3] = vec2[](GetTriangleData(Triangle,Mesh,3).rg,GetTriangleData(Triangle+1,Mesh,3).rg,GetTriangleData(Triangle+2,Mesh,3).rg); 
		vec3 StoredNormals[3] = vec3[](GetTriangleData(Triangle,Mesh,1),GetTriangleData(Triangle+1,Mesh,1),GetTriangleData(Triangle+2,Mesh,1)); 

		//barycentric interpolation
		float RatioP1 = Barycentric(vec2(1.0,0.0),vec2(0.0,1.0),UV); 
		float RatioP2 = Barycentric(vec2(0.0,0.0),vec2(0.0,1.0),UV); 
		float RatioP3 = Barycentric(vec2(0.0,0.0),vec2(1.0,0.0),UV);

		vec2 UV = StoredUVC[0] * RatioP1 + StoredUVC[1] * RatioP2 + StoredUVC[2] * RatioP3; 

		vec3 Normal = normalize(StoredNormals[0] * RatioP1 + StoredNormals[1] * RatioP2 + StoredNormals[2] * RatioP3); 

		int Index = int(floor(GetTriangleData(Triangle,Mesh,3).z * 100.0 + .1f)) * 2;
		vec3 RawSample1 = texelFetch(ModelMaterials, ivec2(Index, 0), 0).xyz; 
		vec3 RawSample2 = texelFetch(ModelMaterials, ivec2(Index+1, 0), 0).xyz; 
		float Metal = 0.; 
		float Roughness = .1; 


		if(RawSample2.y < 0.) {
			
			int MaterialTexture = int(floor(RawSample2.x * 128.f + .1f)); 
			vec3 TextureCoordinate = vec3(UV,float(MaterialTexture)); 

			IndirectHit = texture(LowResTextures, TextureCoordinate); 
		}
		else {
			


			IndirectHit = vec4(RawSample1,1.); 
			Roughness = RawSample2.y; 
			Metal = RawSample2.z; 
		}
		


		Hit = vec4(RAY.Origin + RAY.Direction * Traversal, Metal); //store metal component in alpha value of the hit 
		Direction = vec4(RAY.Direction, 0.); 
		NormalHit = vec4(Normal, Roughness); 


		Result += vec4(IndirectHit.xyz * GetDirectionalShadowColor(Hit.xyz, 2, SchlicksAproximation(RAY.Direction, Normal, Metal, Roughness), Roughness, Normal, RAY.Direction), 0.); 
		

	}
	else { 
		//if not, we flew into the sky

		vec3 SkyColor = GetSkyColor(RAY.Direction); 

		IndirectHit = vec4(SkyColor,1.); 
		Result += IsSecondaryRay ? vec4(texture(PreviousIndirectHit, TexCoord).xyz * SkyColor,1.) : vec4(SkyColor,1.); 
		Hit = vec4(-1.); 
	}
	OutPosition = PositionRaw; //useful for upscaling
	OutTrace = vec4(Result.xyz * AO,IsDiffuseRay ? 1. : -1.); //required for the temporal filter down the line 
	
}

