#version 420 core

in vec3 f_pos;

layout(location = 0) out vec4 frag_color;
layout(location = 1) out vec4 bright_color;

uniform samplerCube sampler;

void main() {
    vec3 color = textureLod(sampler, f_pos, 0.0).rgb;

    // tonemapping + gamma correction
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    frag_color = vec4(color, 1.0);

    float brightness = dot(frag_color.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0) {
        bright_color = vec4(frag_color.rgb, 1.0);
    } else {
        bright_color = vec4(0, 0, 0, 1);
    }
}
