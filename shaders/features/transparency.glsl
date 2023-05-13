// weigth and blend transparent material
if (out_color.a < 1) {
    // weight function
    float weight = clamp(pow(min(1.0, out_color.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);
    // store pixel revealage threshold
    out_reveal = out_color.a;
    // store pixel color accumulation
    out_color = vec4(out_color.rgb * out_color.a, out_color.a) * weight;
}
