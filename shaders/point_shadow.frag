#version 460 core

in vec4 f_pos;

uniform vec3 light_pos;
uniform float far_plane;

void main()
{
    float d = length(f_pos.xyz - light_pos);
    d = d / far_plane; // normalizes distance value into range [0, 1]
    gl_FragDepth = d;
}