#ifndef LIGHTING_HEADER
#define LIGHTING_HEADER

#include "_brdf.glsl"

struct DirectionalLight{
    // (R,G,B,ambient)
    vec4 intensity;
    // (X,Y,Z,distance to camera)
    vec4 direction;

    // (resolution, far1, far2, far3)
    vec4 status;
    mat4 proj_view_1;
    mat4 proj_view_2;
    mat4 proj_view_3;

    // bool has_shadow;
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
    float depth;        // depth to camera.
    float linear_depth;        // Linear depth to camera.
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

float linearizeDepth(float depth, float near, float far)
{
    vec2 inv = vec2(1.0 / near, 1.0 / far);
    return 1.0 / (inv.r - depth * (inv.r - inv.g));    
}

bool posFromLight(mat4 proj_view, vec3 position, out vec3 ndc){
    vec4 fromLight = proj_view * vec4(position, 1.0);
    ndc = fromLight.xyz / fromLight.w;
    ndc.xy = ndc.xy * 0.5 + 0.5;
    if(min(ndc.x, ndc.y) <= 0.0 || max(ndc.x, ndc.y) >= 1.0){
        return false;
    }
    return true;
}

float dirlightVisibility(DirectionalLight dir_light, sampler2D shadowmap, mat4 proj_view, vec3 position, float bias){
    vec3 ndc;
    if(!posFromLight(proj_view, position, ndc)){
        return 1.0;
    }
    float mindep = texture(shadowmap, ndc.xy).r;
    float zReceiver = ndc.z;
    return step(zReceiver, mindep + bias);
}

vec3 evalLight(ShadingData sd, LightSample ls){
    vec3 color = vec3(0.0);
    vec3 brdf = sd.kD * sd.baseColor * M_1_PI + sd.kS * BRDF(sd.specular, sd.roughness, ls.NdotH, ls.NdotL, sd.NdotV, ls.LdotH);
    color += brdf * ls.intensity * ls.NdotL;
    return color;
}

vec3 evalDirectionalLightColor(ShadingData sd, DirectionalLight light){
    LightSample ls;
    ls.L = -normalize(light.direction.xyz);
    ls.NdotL = dot(sd.N, ls.L);
    vec3 H = normalize(sd.V + ls.L);
    ls.NdotH = dot(sd.N, H);
    ls.LdotH = dot(ls.L, H);
    ls.intensity = light.intensity.rgb;
    return evalLight(sd, ls);
}

vec3 evalDirectionalLight(ShadingData sd, DirectionalLight light, sampler2D shadowmap, mat4 proj_view){
    vec3 color = evalDirectionalLightColor(sd, light);
    float vis = dirlightVisibility(light, shadowmap, proj_view, sd.pos, 0.001);
    
    return vis * color;
}

#endif