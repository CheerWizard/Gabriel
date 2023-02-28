#version 420 core

out vec4 frag_color;

in vec2 f_uv;

uniform sampler2D sampler;

uniform float exposure;
uniform float gamma;

void main()
{
    vec3 hdr = texture(sampler, f_uv).rgb;
    // screen_exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdr * exposure);
    // screen_gamma correction
    mapped = pow(mapped, vec3(1.0 / gamma));
    frag_color = vec4(mapped, 1.0);
}