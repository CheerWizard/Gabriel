struct LightPhong {
    uint id;
    vec3 position;
    vec4 color;
};

layout (std430, binding = 1) buffer PhongLightBuffer {
    LightPhong phongLights[];
};
