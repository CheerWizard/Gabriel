#version 460 core

layout(location = 0) out vec4 out_color;
layout(location = 2) out uvec3 out_ids;

uniform vec4 color;
uniform uint object_id;
uniform uint draw_id;

void main()
{
    out_color = color;
    out_ids = uvec3(object_id, draw_id, gl_PrimitiveID);
}