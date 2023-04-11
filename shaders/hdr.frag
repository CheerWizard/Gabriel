#version 460 core

out vec3 out_color;

in vec2 l_uv;

uniform sampler2D scene;
uniform sampler2D shiny;

uniform float exposure;
uniform float shiny_strength;

void main()
{
    vec3 color = texture(scene, l_uv).rgb;
    // HDR tone mapping
    color = vec3(1.0) - exp(-color * exposure);
    // apply shiny lens effect
    vec3 shiny_color = texture(shiny, vec2(l_uv.x, 1.0f - l_uv.y)).rgb;
    color = mix(color, shiny_color, color * vec3(shiny_strength / 100.0f));

    out_color = color;
}