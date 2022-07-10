#version 450

layout(set = 2, binding = 0) uniform _unused_name_permaterial
{
    uint flags;
    vec4 baseColorFactor;
    vec4 specularFactor;
    vec4 normalFactor;
    vec4 emissiveFactor;
}mat_vars;

layout(set = 2, binding = 1) uniform sampler2D baseColor;
layout(set = 2, binding = 2) uniform sampler2D specular;
layout(set = 2, binding = 3) uniform sampler2D normal;
layout(set = 2, binding = 4) uniform sampler2D emissive;

layout(location = 0) in vec4 fragWorldPos;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in mat3 fragTBN;

layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outBaseColor;
layout(location = 3) out vec4 outSpecular;

#define USE_SHADOWMAP   true
#define BASECOLOR_BIT   0x00000001
#define SPECULAR_BIT    0x00000002
#define NORMAL_BIT      0x00000004
#define EMISSIVE_BIT    0x00000008

void main() {
    outPosition = fragWorldPos;

    if((mat_vars.flags & BASECOLOR_BIT) != 0){
        outBaseColor = texture(baseColor, fragTexCoord);
    }
    else{
        outBaseColor = mat_vars.baseColorFactor;
    }

    if((mat_vars.flags & SPECULAR_BIT) != 0){
        outSpecular = texture(specular, fragTexCoord);
    }
    else{
        outSpecular = mat_vars.specularFactor;
    }
    
    if((mat_vars.flags & NORMAL_BIT) != 0 && USE_SHADOWMAP){
        //vec2 normal_rg = texture(normal, fragTexCoord).rg;
        //N.z = sqrt(1.0 - clamp(dot(N, N), 0.0, 1.0));
        vec3 N = texture(normal, fragTexCoord).rgb;
        N = pow(N, vec3(1.0 / 2.2));
        N.xy = N.xy * 2.0 - 1.0;
        N = normalize(fragTBN * N);
        outNormal = vec4(N, 1.0);
    }
    else{
        vec3 fragNormal = normalize(fragTBN[2]);
        outNormal = vec4(fragNormal, 1.0);
    }
}