#version 410 // -*- c++ -*-
#include <reverseReprojection.glsl>
#include <Texture/Texture.glsl>

#expect USE_VIRTUAL_DISTANCE
#expect USE_COLOR_CLIPPING
#expect REJECT_IF_VERY_DIFFERENT
#expect SAVE_WEIGHT


uniform_Texture(sampler2D, unfilteredValue_);
uniform_Texture(sampler2D, ssVelocity_);
uniform_Texture(sampler2D, depth_);
uniform_Texture(sampler2D, previousDepth_);
uniform_Texture(sampler2D, previousValue_);

uniform mat4x3 cameraToWorld;
uniform mat4x3 cameraToWorldPrevious;
uniform mat4 worldToScreenPrevious;
uniform vec3 clipInfo;
uniform vec4 projInfo;
uniform float hysteresis;
uniform int neighbourhoodRadius;
uniform vec2 guardBandSize;

uniform float falloffStartDistance;
uniform float falloffEndDistance;

layout(location = 0) out float4 result;

#if SAVE_WEIGHT
layout(location = 1) out float weight_out;
#endif


vec3 RGBToYCoCg(vec3 rgb) {
    float Y = dot(rgb, vec3(1, 2, 1));
    float Co = dot(rgb, vec3(2, 0, -2));
    float Cg = dot(rgb, vec3(-1, 2, -1));

    vec3 YCoCg = float3(Y, Co, Cg);
    return YCoCg;
}

vec3 YCoCgToRGB(vec3 YCoCg) {
    float Y = YCoCg.x * 0.25;
    float Co = YCoCg.y * 0.25;
    float Cg = YCoCg.z * 0.25;

    float R = Y + Co - Cg;
    float G = Y + Cg;
    float B = Y - Co - Cg;

    return vec3(R, G, B);
}



void main() {
    vec2 screenCoord = gl_FragCoord.xy;
    ivec2 C = ivec2(screenCoord);
    vec4 currentVal = texelFetch(unfilteredValue_buffer, C, 0);
    vec2 ssV = texelFetch(ssVelocity_buffer, C, 0).rg * ssVelocity_readMultiplyFirst.xy + ssVelocity_readAddSecond.xy;

    float depth = texelFetch(depth_buffer, C, 0).r;
    vec3 currentCSPosition = reconstructCSPositionFromDepth(screenCoord, depth, projInfo, clipInfo);

#if USE_VIRTUAL_DISTANCE
    float secondaryRayDistance = min(currentVal.a, 1000);
    vec3 virtualCSPosition = normalize(currentCSPosition)*secondaryRayDistance + currentCSPosition;
    vec3 virtualWSPosition = cameraToWorld * vec4(virtualCSPosition, 1.0);
#else
    vec3 virtualWSPosition = cameraToWorld * vec4(currentCSPosition, 1.0);
#endif

    vec2 previousCoord = screenCoord - ssV;
    if (previousCoord.x < guardBandSize.x || previousCoord.y < guardBandSize.y ||
        previousCoord.x > unfilteredValue_size.x - guardBandSize.x || 
        previousCoord.y > unfilteredValue_size.y - guardBandSize.y) { 
        // outside guard band
        result = currentVal;
        return;
    }

    vec4 prevProjCoord = (worldToScreenPrevious * vec4(virtualWSPosition, 1.0));
    vec2 previousNormalizedCoord = (prevProjCoord.xy / prevProjCoord.w) * unfilteredValue_invSize.xy;
    vec4 previousVal = texture(previousValue_buffer, previousNormalizedCoord);

#if USE_COLOR_CLIPPING
    vec3 boxMin = RGBToYCoCg(currentVal.rgb);
    vec3 boxMax = RGBToYCoCg(currentVal.rgb);
    for (int y = -neighbourhoodRadius; y <= neighbourhoodRadius; ++y) {
        for (int x = -neighbourhoodRadius; x <= neighbourhoodRadius; ++x) {
            vec3 val = RGBToYCoCg(texelFetch(unfilteredValue_buffer, C + ivec2(x, y), 0).rgb);
            boxMin = min(boxMin, val);
            boxMax = max(boxMax, val);
        }
    }

    vec3 yCoCgPrev = RGBToYCoCg(previousVal.rgb);
    // Could do a clip instead of a clamp here.
    yCoCgPrev = min(boxMax, yCoCgPrev);
    yCoCgPrev = max(boxMin, yCoCgPrev);
    previousVal.rgb = YCoCgToRGB(yCoCgPrev);
#endif

    float weight = hysteresis;
#if REJECT_IF_VERY_DIFFERENT
    vec3 currentWSPosition = cameraToWorld * vec4(currentCSPosition, 1.0);
    float distance;
    vec4 trash = reverseReprojection(screenCoord, currentWSPosition, ssV, previousValue_buffer,
        previousDepth_buffer, unfilteredValue_invSize.xy, clipInfo, projInfo, cameraToWorldPrevious, distance);

    weight *= (1.0 - smoothstep(falloffStartDistance, falloffEndDistance, distance));
#endif



#if SAVE_WEIGHT
    weight_out = weight;
#endif

#   ifdef DEBUG
    result = vec4(weight, 1.0);
#   else
        result = currentVal;
        result.xyz = weight * previousVal.xyz + (1.0 - weight) * currentVal.xyz;
#   endif
}