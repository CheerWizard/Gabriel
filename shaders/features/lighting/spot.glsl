struct LightSpot {
    uint id;
    float cutoff;
    float outer;
    float refraction;
    vec3 position;
    vec3 direction;
    vec4 color;
};

layout (std430, binding = 4) buffer SpotLightBuffer {
    LightSpot spotLights[];
};
