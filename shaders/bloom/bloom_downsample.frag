#version 460 core

layout (location = 0) out vec3 out_downsample;

in vec2 l_uv;

uniform ivec2 resolution;
uniform sampler2D src;
uniform int mip_level = 1;

vec3 pow_vec3(vec3 v, float p) { return vec3(pow(v.x, p), pow(v.y, p), pow(v.z, p)); }

vec3 to_srgb(vec3 v) { return pow_vec3(v, 1.0 / 2.2); }

float rgb_to_luminance(vec3 col) { return dot(col, vec3(0.299f, 0.587f, 0.114f)); }

float karis_average(vec3 col) {
    float luma = rgb_to_luminance(to_srgb(col)) * 0.25f;
    return 1.0f / (1.0f + luma);
}

void main() {

    vec2 texel_size = 1.0 / resolution;
    float x = texel_size.x;
    float y = texel_size.y;

    vec3 e = texture(src, l_uv).rgb;
    // test brightness of HDR color for blooming
    float brightness = dot(e, vec3(0.2126, 0.7152, 0.0722));
    if (brightness <= 1.0) {
        out_downsample = vec3(0);
        out_downsample = max(out_downsample, 0.0001f); // fix for visual bug with black quads
        return;
    }

    vec3 a = texture(src, vec2(l_uv.x - 2*x, l_uv.y + 2*y)).rgb;
    vec3 b = texture(src, vec2(l_uv.x, l_uv.y + 2*y)).rgb;
    vec3 c = texture(src, vec2(l_uv.x + 2*x, l_uv.y + 2*y)).rgb;
    vec3 d = texture(src, vec2(l_uv.x - 2*x, l_uv.y)).rgb;

    vec3 f = texture(src, vec2(l_uv.x + 2*x, l_uv.y)).rgb;
    vec3 g = texture(src, vec2(l_uv.x - 2*x, l_uv.y - 2*y)).rgb;
    vec3 h = texture(src, vec2(l_uv.x, l_uv.y - 2*y)).rgb;
    vec3 i = texture(src, vec2(l_uv.x + 2*x, l_uv.y - 2*y)).rgb;

    vec3 j = texture(src, vec2(l_uv.x - x, l_uv.y + y)).rgb;
    vec3 k = texture(src, vec2(l_uv.x + x, l_uv.y + y)).rgb;
    vec3 l = texture(src, vec2(l_uv.x - x, l_uv.y - y)).rgb;
    vec3 m = texture(src, vec2(l_uv.x + x, l_uv.y - y)).rgb;

    // Check if we need to perform Karis average on each block of 4 samples
    vec3 groups[5];
    switch (mip_level) {
        case 0:
        // We are writing to mip 0, so we need to apply Karis average to each block
        // of 4 samples to prevent fireflies (very bright subpixels, leads to pulsating
        // artifacts).
        groups[0] = (a+b+d+e) * (0.125f/4.0f);
        groups[1] = (b+c+e+f) * (0.125f/4.0f);
        groups[2] = (d+e+g+h) * (0.125f/4.0f);
        groups[3] = (e+f+h+i) * (0.125f/4.0f);
        groups[4] = (j+k+l+m) * (0.5f/4.0f);
        groups[0] *= karis_average(groups[0]);
        groups[1] *= karis_average(groups[1]);
        groups[2] *= karis_average(groups[2]);
        groups[3] *= karis_average(groups[3]);
        groups[4] *= karis_average(groups[4]);
        out_downsample = groups[0] + groups[1] + groups[2] + groups[3] + groups[4];
        break;

        default:
        out_downsample = e * 0.125;
        out_downsample += (a+c+g+i) * 0.03125;
        out_downsample += (b+d+f+h) * 0.0625;
        out_downsample += (j+k+l+m) * 0.125;
        break;
    }

    out_downsample = max(out_downsample, 0.0001f); // fix for visual bug with black quads
}