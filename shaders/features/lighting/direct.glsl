struct LightDirectional {
    uint id;
    vec3 position;
    vec4 color;
};

layout (std430, binding = 2) buffer DirectLightBuffer {
    LightDirectional directLights[];
};

float directAttenuation(vec3 lightPos, float q, float l, float c) {
    float d = length(lightPos - w_pos);
    return 1.0 / ( q * d * d + l * d + c );
}