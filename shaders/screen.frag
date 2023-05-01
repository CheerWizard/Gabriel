#version 460 core

out vec4 out_color;

in vec2 l_uv;

uniform sampler2D scene;
uniform sampler2D ui;
uniform sampler2D debug_control;

uniform float gamma;

void main()
{
    vec3 screen_color = texture(scene, l_uv).rgb;
    screen_color = pow(screen_color, vec3(1.0 / gamma));
    out_color = vec4(screen_color, 1.0);
}