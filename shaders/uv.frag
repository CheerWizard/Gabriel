#version 330 core

out vec4 fragment;

in vec2 f_uv;

uniform sampler2D sampler1;
uniform sampler2D sampler2;
uniform float mix_factor;

void main()
{
    fragment = mix(texture(sampler1, f_uv), texture(sampler2, f_uv), mix_factor);
}