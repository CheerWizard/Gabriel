#version 460 core

out float out_color;

in vec2 l_uv;

uniform sampler2D ssao;

void main() {
    vec2 texelSize = 1.0 / vec2(textureSize(ssao, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x)
    {
        for (int y = -2; y < 2; ++y)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssao, l_uv + offset).r;
        }
    }
    out_color = result / (4.0 * 4.0);
}