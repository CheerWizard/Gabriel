vec2 parallax_function(vec2 uv)
{
    float height_scale = material.height_scale;
    float min_layers = material.parallax_min_layers;
    float max_layers = material.parallax_max_layers;
    float layers = mix(max_layers, min_layers, abs(dot(vec3(0.0, 0.0, 1.0), V)));
    // calculate the size of each layer
    float layer_depth = 1.0 / layers;
    // depth of current layer
    float current_layer_depth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = V.xy / V.z * height_scale;
    vec2 delta_uv = P / layers;

    vec2 current_uv = uv;
    float current_parallax_value = texture(material.parallax, current_uv).r;
    while (current_layer_depth < current_parallax_value) {
        // shift texture coordinates along direction of P
        current_uv -= delta_uv;
        // get depthmap value at current texture coordinates
        current_parallax_value = texture(material.parallax, current_uv).r;
        // get depth of next layer
        current_layer_depth += layer_depth;
    }

    // get texture coordinates before collision (reverse operations)
    vec2 prev_uv = current_uv + delta_uv;

    // get depth after and before collision for linear interpolation
    float after_depth  = current_parallax_value - current_layer_depth;
    float before_depth = texture(material.parallax, prev_uv).r - current_layer_depth + layer_depth;

    // interpolation of texture coordinates
    float weight = after_depth / (after_depth - before_depth);
    vec2 final_uv = prev_uv * weight + current_uv * (1.0 - weight);

    return final_uv;
}
