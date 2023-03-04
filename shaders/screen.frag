#version 460 core

out vec4 frag_color;

in vec2 f_uv;

uniform sampler2D sampler;

uniform float exposure;
uniform float gamma;

void main()
{
    vec3 color = texture(sampler, f_uv).rgb;
    // HDR tone mapping
    color = vec3(1.0) - exp(-color * exposure);
    // gamma correction
    color = pow(color, vec3(1.0 / gamma));
    frag_color = vec4(color, 1.0);
}