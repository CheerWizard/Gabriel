#version 460 core

#include ../core.glsl

layout(location = 0) out vec3 outColor;

in vec2 l_uv;

uniform sampler2D src;

uniform vec3 inverseFilterSize;
uniform float spanMax = 8.0;
uniform float reduceMin = 1.0 / 128.0;
uniform float reduceMul = 1.0 / 8.0;

void main() {
    vec3 luma = vec3(0.299, 0.587, 0.114);
    vec2 texelOffset = inverseFilterSize.xy;

    float lumaMid = dot(luma, texture(src, l_uv).xyz);
    float lumaTopLeft = dot(luma, texture(src, l_uv + (vec2(-1, -1) * texelOffset)).xyz);
    float lumaTopRight = dot(luma, texture(src, l_uv + (vec2(1, -1) * texelOffset)).xyz);
    float lumaBotLeft = dot(luma, texture(src, l_uv + (vec2(-1, 1) * texelOffset)).xyz);
    float lumaBotRight = dot(luma, texture(src, l_uv + (vec2(1, 1) * texelOffset)).xyz);

    vec2 blurDir;
    blurDir.x = -((lumaTopLeft + lumaTopRight) - (lumaBotLeft + lumaBotRight));
    blurDir.x = ((lumaTopLeft + lumaBotLeft) - (lumaTopRight + lumaBotRight));

    float dirReduce = max((lumaTopLeft + lumaTopRight + lumaBotLeft + lumaBotRight) * (reduceMul * 0.25), reduceMin);
    float inverseBlurDir = 1.0 / (min(abs(blurDir.x), abs(blurDir.y) + dirReduce));
    blurDir = min(vec2(spanMax, spanMax), max(vec2(-spanMax, -spanMax), blurDir * inverseBlurDir)) * texelOffset;

    vec3 result1 = 0.5 * (texture(src, l_uv + (blurDir * vec2(1.0/3.0 - 0.5))).xyz + texture(src, l_uv + (blurDir * vec2(2.0/3.0 - 0.5))).xyz);
    vec3 result2 = result1 * 0.5 + 0.25 * (texture(src, l_uv + (blurDir * vec2(0.0/3.0 - 0.5))).xyz + texture(src, l_uv + (blurDir * vec2(3.0/3.0 - 0.5))).xyz);

    float lumaMin = min(lumaMid, min(min(lumaTopLeft, lumaTopRight), min(lumaBotLeft, lumaBotRight)));
    float lumaMax = max(lumaMid, max(max(lumaTopLeft, lumaTopRight), max(lumaBotLeft, lumaBotRight)));
    float lumaResult = dot(luma, result2);

    if (lumaResult < lumaMin || lumaResult > lumaMax) {
        outColor = result1;
    } else {
        outColor = result2;
    }
}
