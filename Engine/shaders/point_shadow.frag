#version 460 core

in vec4 w_pos;

uniform vec3 light_pos;
uniform float far_plane;

void main()
{
    float d = length(w_pos.xyz - light_pos) / far_plane; // normalizes distance value into range [0, 1]
    gl_FragDepth = d;
}