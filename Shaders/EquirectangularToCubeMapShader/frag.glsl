#version 330
out vec3 Color; 
in vec3 Vertice; 
uniform sampler2D EquirectangularMap; 

//Credit where credit is due, this code isn't mine; It's taken from learnopengl and it works very well from what I've seen 
const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}


void main() {
	Color = texture(EquirectangularMap,SampleSphericalMap(normalize(Vertice))).rgb;
}