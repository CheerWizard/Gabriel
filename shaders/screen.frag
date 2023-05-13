#version 460 core

#include core.glsl

out vec4 outColor;

in vec2 l_uv;

uniform sampler2D scene;
uniform sampler2D ui;

#ifdef IMGUI
uniform sampler2D visuals;
#endif

uniform float gamma;

void main()
{
    vec3 screenColor = vec3(0, 0, 0);

    vec3 sceneColor = texture(scene, l_uv).rgb;
    sceneColor = pow(sceneColor, vec3(1.0 / gamma));
    screenColor += sceneColor;

    vec3 uiColor = texture(ui, l_uv).rgb;
    screenColor += uiColor;

    #ifdef IMGUI
    vec3 visualsColor = texture(visuals, l_uv).rgb;
    screenColor += visualsColor;
    #endif

    outColor = vec4(screenColor, 1.0);
}