#version 460 core

layout (location = 0) out vec3 out_color;

in vec2 l_uv;

uniform sampler2D hdr;
uniform sampler2D bloom;

uniform float bloom_strength = 0.04f;

void main() {
    vec3 hdr_color = texture(hdr, l_uv).rgb;
    vec3 bloom_color = texture(bloom, l_uv).rgb;
    out_color = mix(hdr_color, bloom_color, vec3(bloom_strength));
}
