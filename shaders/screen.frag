#version 460 core

#include core.glsl

out vec4 outColor;

in vec2 l_uv;

uniform sampler2D scene;

uniform float gamma;

void main()
{
    vec3 screenColor = texture(scene, l_uv).rgb;
    // tone mapping
    screenColor = pow(screenColor, vec3(1.0 / gamma));
    outColor = vec4(screenColor, 1.0);
}