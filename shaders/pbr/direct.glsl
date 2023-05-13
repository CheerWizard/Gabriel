vec3 pbr(vec3 L, vec3 light_color, float radiance_factor, vec3 albedo, float metallic, float roughness)
{
    vec3 H = normalize(V + L);
    vec3 radiance = light_color * radiance_factor;

    // diffuse refraction/specular reflection ratio on surface
    vec3 F0 = vec3(0.04); // base reflection 0.04 covers most of dielectrics
    F0      = mix(F0, albedo, metallic); // F0 = albedo color if it's a complete metal
    vec3 F  = fresnel_shlick(max(dot(H, V), 0), F0);

    // normal distribution halfway vector along rough surface
    float D = distribution_ggx(H, roughness);

    // geometry obstruction and geometry shadowing of microfacets
    float G = geometry_smith(V, L, roughness);

    // BRDF Cook-Torrance approximation
    float y = 0.0001; // infinetely small number used when dot product N/V or N/L returns 0
    vec3 specular = (D * G * F) / ( 4.0 * max(dot(N, V), 0) * max(dot(N, L), 0) + y);

    // light contribution to reflectance equation
    vec3 ks = F;
    vec3 kd = vec3(1.0) - ks;
    kd *= 1.0 - metallic;

    // diffuse absorption/scatter using Lambert function
    vec3 diffuse = lambert(kd, albedo);

    // solve reflectance equation
    return (diffuse + specular) * radiance * max(dot(N, L), 0);
}

vec3 pbr(LightPoint lightPoint, vec3 albedo, float metallic, float roughness)
{
    vec3 lightPos = lightPoint.position;
    vec3 lightDir = normalize(lightPos - w_pos);
    vec3 lightColor = lightPoint.color.rgb * lightPoint.color.a;
    float constant  = lightPoint.constant;
    float linear    = lightPoint.linear;
    float quadratic = lightPoint.quadratic;
    float A = pointAttenuation(lightPos, quadratic, linear, constant);
    float pointShadow = point_shadow_function(lightPos);
    float radianceFactor = A * (1.0 - pointShadow);

    return pbr(lightDir, lightColor, radianceFactor, albedo, metallic, roughness);
}

vec3 pbr(LightSpot lightSpot, vec3 albedo, float metallic, float roughness)
{
    vec3 lightPos = lightSpot.position;
    vec3 lightDir = normalize(lightPos - w_pos);
    vec3 lightColor = lightSpot.color.rgb * lightSpot.color.a;
    vec3 spotDir       = normalize(-lightSpot.direction);
    float theta         = dot(lightDir, spotDir);
    float cutoff        = lightSpot.cutoff;
    float gamma         = lightSpot.outer;
    float epsilon       = cutoff - gamma;
    float I             = clamp((theta - gamma) / epsilon, 0.0, 1.0);
    float radianceFactor = I * (1.0);

    return pbr(lightDir, lightColor, radianceFactor, albedo, metallic, roughness);
}

vec3 pbr(LightDirectional lightDirect, vec3 albedo, float metallic, float roughness)
{
    vec3 lightPos = lightDirect.position;
    vec3 lightColor = lightDirect.color.rgb * lightDirect.color.a;
    float A = directAttenuation(lightPos, 1, 0, 0);
    float directShadow = direct_shadow_function(lightPos);
    float radianceFactor = A * (1.0 - directShadow);
    return pbr(lightPos, lightColor, radianceFactor, albedo, metallic, roughness);
}