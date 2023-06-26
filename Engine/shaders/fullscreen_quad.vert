#version 460 core

out vec2 l_uv;

void main()
{
    float u = float(((uint(gl_VertexID) + 2u) / 3u)%2u);
    float v = float(((uint(gl_VertexID) + 1u) / 3u)%2u);
    gl_Position = vec4(u * 2.0f - 1.0f, v * 2.0f - 1.0f, 0.0f, 1.0f);
    l_uv = vec2(u, v);
}