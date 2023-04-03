#version 460 core

out vec2 l_uv;

void main()
{
    float x = float(((uint(gl_VertexID) + 2u) / 3u)%2u);
    float y = float(((uint(gl_VertexID) + 1u) / 3u)%2u);
    gl_Position = vec4(x * 2.0f - 1.0f, y * 2.0f - 1.0f, 0.0f, 1.0f);
    l_uv = vec2(x, y);
}