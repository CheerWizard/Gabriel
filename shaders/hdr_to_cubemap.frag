#version 460 core

out vec4 out_color;

in vec3 l_pos;

uniform sampler2D sampler;

const vec2 inv_atan = vec2(0.1591, 0.3183);

vec2 sample_sphere(vec3 v) {
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= inv_atan;
    uv += 0.5;
    return uv;
}

void main() {
    vec2 uv = sample_sphere(normalize(l_pos));
    out_color = vec4(texture(sampler, uv).rgb, 1.0);
}