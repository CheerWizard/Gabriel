layout (std140, binding = 0) uniform Camera {
    mat4 perspective;
    mat4 view;
    vec3 camera_pos;
};