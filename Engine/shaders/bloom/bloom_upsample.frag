#version 460 core

layout (location = 0) out vec3 out_upsample;

in vec2 l_uv;

uniform float filter_radius;
uniform sampler2D src;

void main() {

    float x = filter_radius;
    float y = filter_radius;

    vec3 e = texture(src, l_uv).rgb;

    vec3 a = texture(src, vec2(l_uv.x - x, l_uv.y + y)).rgb;
    vec3 b = texture(src, vec2(l_uv.x, l_uv.y + y)).rgb;
    vec3 c = texture(src, vec2(l_uv.x + x, l_uv.y + y)).rgb;

    vec3 d = texture(src, vec2(l_uv.x - x, l_uv.y)).rgb;
    vec3 f = texture(src, vec2(l_uv.x + x, l_uv.y)).rgb;

    vec3 g = texture(src, vec2(l_uv.x - x, l_uv.y - y)).rgb;
    vec3 h = texture(src, vec2(l_uv.x, l_uv.y - y)).rgb;
    vec3 i = texture(src, vec2(l_uv.x + x, l_uv.y - y)).rgb;

    out_upsample = e * 4.0;

    out_upsample += (b + d + f + h) * 2.0;
    out_upsample += (a + c + g + i) * 1.0;
    out_upsample *= 1.0 / 16.0;
}
