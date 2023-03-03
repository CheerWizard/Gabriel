#version 420 core

out vec4 frag_color;

in vec3 f_pos;

uniform samplerCube sampler;
uniform float roughness;
uniform float resolution;

const float PI = 3.14159265359;
const uint SAMPLES = 1024u;

vec3 N;
vec3 V;
vec3 R;

float van_der_corput(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 hammersley(uint i, uint n)
{
    return vec2(float(i) / float(n), van_der_corput(i));
}

vec3 importance_sampling_ggx(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness * roughness;

    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    // from tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

float distribution_ggx(vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NH = max(dot(N, H), 0);
    float NH2 = NH * NH;
    float x = (NH2 * (a2 - 1.0) + 1.0);

    return a2 / (PI * x * x);
}

void main()
{
    N = normalize(f_pos);
    R = N;
    V = R;

    float total_weigth = 0.0;
    vec3 color = vec3(0.0);

    for (uint i = 0u; i < SAMPLES; i++)
    {
        vec2 Xi = hammersley(i, SAMPLES);
        vec3 H  = importance_sampling_ggx(Xi, N, roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0)
        {
            // sample from the environment's mip level based on roughness/pdf
            float D = distribution_ggx(H, roughness);
            float NdotH = max(dot(N, H), 0.0);
            float HdotV = max(dot(H, V), 0.0);
            float pdf = D * NdotH / (4.0 * HdotV) + 0.0001;

            float saTexel  = 4.0 * PI / (6.0 * resolution * resolution);
            float saSample = 1.0 / (float(SAMPLES) * pdf + 0.0001);

            float mip = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);

            color += textureLod(sampler, L, mip).rgb * NdotL;
            total_weigth += NdotL;
        }
    }

    color = color / total_weigth;

    frag_color = vec4(color, 1.0);
}