#ifndef LIGHTING_HEADER
#define LIGHTING_HEADER

#include "_brdf.glsl"

struct DirectionalLight{
    // (R,G,B,ambient)
    vec4 intensity;
    // (X,Y,Z,_)
    vec4 direction;

    // bool has_shadow;
    // mat4 viewProj;
    // float frustum, range_z;
    // sampler2D shadowMap;
    // float resolution;
    // float light_size;
};

struct PointLight{
    // (R,G,B,ambient)
    vec4 intensity;
    // (X,Y,Z,_)
    vec4 position;
    // (X,Y,Z,_)
    vec4 direction;

    // float range;
    // float opening_angle, cos_opening_angle;
    // float penumbra_angle;

    // float constant;
    // float linear;
    // float quadratic;

    // bool has_shadow;
    // mat4 viewProj;
    // float inv_n, inv_f;
    // sampler2D shadowMap;
    // float resolution;
    // float light_size;
};

struct LightSample{
    vec3 intensity;
    vec3 L;         // The direction from the surface to the light source
    float NdotH;
    float NdotL;
    float LdotH;
};

struct ShadingData{
    vec3 pos;           // Shading hit position in world space
    vec3 V;             // Direction to the eye at shading hit
    vec3 N;             // Shading normal at shading hit
    vec3 R;
    float NdotV;        // Unclamped, can be negative.
    float depth;        // Linear depth to camera.
    vec3 baseColor;       
    float ao;           // Ambient Occlusion.
    float roughness;    // This is the original roughness, before re-mapping.
    float metallic;     // Metallic parameter, blends between dielectric and conducting BSDFs.
    vec3 diffuse;       ///< Diffuse albedo.
    vec3 specular;      ///< Specular albedo.
    float ggxAlpha;     // DEPRECATED: This is the re-mapped roughness value, which should be used for GGX computations. It equals `roughness^2`
    vec3 kD;
    vec3 kS;
};

vec3 evalLight(ShadingData sd, LightSample ls){
    vec3 color = vec3(0.0);
    vec3 brdf = sd.kD * sd.baseColor * M_1_PI + sd.kS * BRDF(sd.specular, sd.roughness, ls.NdotH, ls.NdotL, sd.NdotV, ls.LdotH);
    color += brdf * ls.intensity * ls.NdotL;
    return color;
}

vec3 evalDirectionalLight(ShadingData sd, DirectionalLight light){
    LightSample ls;
    ls.L = -normalize(light.direction.xyz);
    ls.NdotL = dot(sd.N, ls.L);
    vec3 H = normalize(sd.V + ls.L);
    ls.NdotH = dot(sd.N, H);
    ls.LdotH = dot(ls.L, H);
    ls.intensity = light.intensity.rgb;
    
    return evalLight(sd, ls);
}

#endif