#version 330 core

out vec4 fragment;

in vec3 f_pos;
in vec2 f_uv;
in vec3 f_normal;
in vec3 f_light_pos;

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

uniform Light light;
uniform Material material;
uniform float time;
uniform vec3 emission_color;

void main()
{
    // todo testing using only specular map
    vec3 diffuse_color = texture(material.specular, f_uv).rgb;
    vec3 specular_color = vec3(1.0) - texture(material.specular, f_uv).rgb;

    vec3 ambience = light.ambient * diffuse_color;

    vec3 N = normalize(f_normal);
    vec3 L = normalize(f_light_pos - f_pos);
    float diff_factor = max(dot(N, L), 0);
    vec3 diffuse = diff_factor * light.diffuse * diffuse_color;

    vec3 V = normalize(-f_pos);
    vec3 R = reflect(-L, N);
    float spec_factor = pow(max(dot(V, R), 0), material.shininess);
    vec3 specular = spec_factor * light.specular * specular_color;

    vec3 emission = vec3(0.0);
    if (diffuse_color.r == 0.0) {
        // moving
        vec2 emission_offset = vec2(0.0, time);
        emission = texture(material.emission, f_uv + emission_offset).rgb * emission_color;
        // fading
        emission = emission * (sin(time) * 0.5 + 0.5) * 2.0;
    }

    vec3 fragment_color = ambience + diffuse + specular + emission;
    fragment = vec4(fragment_color, 1.0);
}