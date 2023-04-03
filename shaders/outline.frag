#version 460 core

layout(location = 0) out vec4 out_color;

uniform vec4 outline_color;

void main() {
    out_color = outline_color;
}
