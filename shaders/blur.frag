#version 460 core

out vec4 frag_color;

in vec2 f_uv;

uniform float offset;
uniform sampler2D sampler;

void main() {

    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right
    );

    float kernel[9] = float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16
    );

    vec3 samples[9];
    for (int i = 0; i < 9; i++) {
        samples[i] = vec3(texture(sampler, f_uv + offsets[i]));
    }

    vec3 blur = vec3(0);
    for (int i = 0; i < 9; i++)
        blur += samples[i] * kernel[i];

    frag_color = vec4(blur, 1.0);

}
