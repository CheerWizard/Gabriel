struct LightPoint {
    uint id;
    vec3 position;
    vec4 color;
    float constant;
    float linear;
    float quadratic;
    float refraction;
};

layout (std430, binding = 3) buffer PointLightBuffer {
    LightPoint pointLights[];
};

float pointAttenuation(vec3 lightPos, float q, float l, float c) {
    float d = length(lightPos - w_pos);
    return 1.0 / ( q * d * d + l * d + c );
}