#version 460 core

layout (location = 0) out vec4 outColor;

in vec2 l_uv;

uniform sampler2D accum;
uniform sampler2D reveal;

const float EPSILON = 0.00001f;
bool approxEqual(float a, float b) {
    return abs(a - b) <= (abs(a) < abs(b) ? abs(b) : abs(a)) * EPSILON;
}

void main() {
    float revealage = texture(reveal, l_uv).r;
    if (approxEqual(revealage, 1.0f))
        discard;

    vec4 accumulation = texture(accum, l_uv);
    accumulation.rgb = abs(accumulation.rgb);

    float m = max(max(accumulation.r, accumulation.g), accumulation.b);
    if (isinf(m))
        accumulation.rgb = vec3(accumulation.a);

    vec3 average = accumulation.rgb / max(accumulation.a, EPSILON); // prevent floating point precision bug

    outColor = vec4(average, 1.0f - revealage);
}