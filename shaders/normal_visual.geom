#version 460

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in NormalVertex {
    vec3 a_normal;
} vertex_in[];

layout (std140, binding = 0) uniform Camera {
    mat4 perspective;
    mat4 view;
};

uniform float length = 0.05f;

void EmitNormalLine(int i) {
    gl_Position = perspective * view * gl_in[i].gl_Position;
    EmitVertex();

    gl_Position = perspective * view * (gl_in[i].gl_Position + length * vec4(vertex_in[i].a_normal, 0.0f));
    EmitVertex();

    EndPrimitive();
}

void main() {
    EmitNormalLine(0);
    EmitNormalLine(1);
    EmitNormalLine(2);
}