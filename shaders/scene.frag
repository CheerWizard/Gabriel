#version 330 core

out vec4 fragment;

in vec2 f_uv;

uniform sampler2D sampler;

void main()
{
    fragment = texture(sampler, f_uv);
}