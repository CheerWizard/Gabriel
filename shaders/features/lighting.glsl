struct LightPhong {
    uint id;
    vec3 position;
    vec4 color;
};

struct LightDirectional {
    uint id;
    vec3 position;
    vec4 color;
};

struct LightPoint {
    uint id;
    vec3 position;
    vec4 color;
    float constant;
    float linear;
    float quadratic;
    float refraction;
};

struct LightSpot {
    uint id;
    float cutoff;
    float outer;
    float refraction;
    vec3 position;
    vec3 direction;
    vec4 color;
};

#define PHONG_LIGHT_CAPACITY 1
#define DIRECT_LIGHT_CAPACITY 1
#define POINT_LIGHT_CAPACITY 100
#define SPOT_LIGHT_CAPACITY 100

layout (std430, binding = 1) buffer PhongLightBuffer {
    LightPhong phongLights[];
};

layout (std430, binding = 2) buffer DirectLightBuffer {
    LightDirectional directLights[];
};

layout (std430, binding = 3) buffer PointLightBuffer {
    LightPoint pointLights[];
};

layout (std430, binding = 4) buffer SpotLightBuffer {
    LightSpot spotLights[];
};