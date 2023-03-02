#version 420 core

in vec3 f_pos;

out vec4 frag_color;

uniform sampler2D sampler;

const vec2 inv_atan = vec2(0.1591, 0.3183);

vec2 sample_sphere(vec3 v) {
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= inv_atan;
    uv += 0.5;
    return uv;
}

void main() {
    vec2 uv = sample_sphere(normalize(f_pos));
    frag_color = vec4(texture(sampler, uv).rgb, 1.0);
}