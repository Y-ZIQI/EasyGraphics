#ifndef BRDF_HEADER
#define BRDF_HEADER

#include "_constants.glsl"

vec3 F_Schlick(vec3 f0, vec3 f90, float u)
{
    return f0 + (f90 - f0) * pow(1 - u, 5);
}

float D_GGX(float ggxAlpha, float NdotH)
{
    float a2 = ggxAlpha * ggxAlpha;
    float d = ((NdotH * a2 - NdotH) * NdotH + 1.0);
    return M_1_PI * a2 / (d * d);
}

float G_SmithGGX(float NdotL, float NdotV, float roughness)
{
    NdotV = max(0.0, NdotV);
    float r  = (roughness + 1.0);
    float k  = (r * r) / 8.0;
    float GL = NdotL / (NdotL * (1.0 - k) + k);
    float GV = NdotV / (NdotV * (1.0 - k) + k);
    return GL * GV;
}

vec3 BRDF(vec3 F0, float roughness, float NdotH, float NdotL, float NdotV, float LdotH){
    float ggxAlpha = max(0.0064, roughness * roughness);
    float D = D_GGX(ggxAlpha, NdotH);
    float G = G_SmithGGX(NdotL, NdotV, roughness);
    vec3 F = F_Schlick(F0, vec3(1.0), clamp(LdotH, 0.0, 1.0));
    return D * G * F / (4.0 * NdotL * NdotV + 0.001);
}

#endif