#version 460 core

layout (location = 0) out vec4 out_color;

in vec2 l_uv;

layout (binding = 0) uniform sampler2D accum;
layout (binding = 1) uniform sampler2D reveal;

const float EPSILON = 0.00001f;
bool approx_equal(float a, float b) {
    return abs(a - b) <= (abs(a) < abs(b) ? abs(b) : abs(a)) * EPSILON;
}

void main() {
    ivec2 uv = ivec2(l_uv.xy);

    float revealage = texelFetch(reveal, uv, 0).r;
    if (approx_equal(revealage, 1.0f))
        discard;

    vec4 accumulation = texelFetch(accum, uv, 0);
    accumulation.rgb = abs(accumulation.rgb);

    float m = max(max(accumulation.r, accumulation.g), accumulation.b);
    if (isinf(m))
        accumulation.rgb = vec3(accumulation.a);

    vec3 average_color = accumulation.rgb / max(accumulation.a, EPSILON); // prevent floating point precision bug

    out_color = vec4(average_color, 1.0f - revealage);
}