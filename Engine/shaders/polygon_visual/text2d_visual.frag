#version 460 core

layout(location = 0) out vec4 out_color;

uniform vec3 color;

void main() {
    out_color = vec4(color, 1.0);
}