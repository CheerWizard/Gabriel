#version 460 core

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_uv;

out vec2 l_uv;

struct Transform2d {
    vec2 translation;
    float rotation;
    vec2 scale;
};

uniform Transform2d transform;

void main() {
    // translate
    float x = a_pos.x + transform.translation.x;
    float y = a_pos.y + transform.translation.y;
    // rotate
    float theta = transform.rotation;
    float rx = x * cos(theta) - y * sin(theta);
    float ry = x * sin(theta) + y * cos(theta);
    // scale
    float sx = rx * transform.scale.x;
    float sy = ry * transform.scale.y;

    vec2 w_pos = vec2(sx, sy);
    gl_Position = vec4(w_pos, 0, 1);

    l_uv = a_uv;
}