#version 450

#include "_lighting.glsl"

layout(set = 0, binding = 0) uniform GlobalUniforms
{
    mat4 proj_view;
    vec3 camera_pos;
    DirectionalLight dir_light;
}global_vars;
layout(set = 0, binding = 1) uniform sampler2D dirlightShadowMap_1;
layout(set = 0, binding = 2) uniform sampler2D dirlightShadowMap_2;
layout(set = 0, binding = 3) uniform sampler2D dirlightShadowMap_3;

layout(set = 1, binding = 0) uniform sampler2D positionTex;
layout(set = 1, binding = 1) uniform sampler2D normalTex;
layout(set = 1, binding = 2) uniform sampler2D baseColorTex;
layout(set = 1, binding = 3) uniform sampler2D specularTex;
layout(set = 1, binding = 4) uniform sampler2D depthTex;

layout(location = 0) in vec2 fragCoord;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

vec3 shading(vec4 position, vec4 normal, vec4 baseColor, vec4 specular, float depth){
    ShadingData sd;
    sd.pos = position.xyz;
    sd.V = normalize(global_vars.camera_pos - sd.pos);
    sd.N = normalize(normal.xyz);
    sd.R = reflect(-sd.V, sd.N);
    sd.NdotV = max(dot(sd.V, sd.N), 0.0);
    sd.depth = depth;
    sd.baseColor = baseColor.rgb;
    sd.ao = 1.0 - specular.r;
    sd.roughness = specular.g;
    sd.metallic = specular.b;
    
    float IoR = 1.5;
    float f = (IoR - 1.0) / (IoR + 1.0);
    vec3 F0 = vec3(f * f);
    sd.diffuse = mix(sd.baseColor, vec3(0), sd.metallic);
    sd.specular = mix(F0, sd.baseColor, sd.metallic);
    sd.ggxAlpha = max(0.0064, sd.roughness * sd.roughness);

    vec3 F = F_Schlick(sd.specular, vec3(1.0), sd.NdotV);
    sd.kD = (vec3(1.0) - F) * (1.0 - sd.metallic);
    sd.kS = 1.0 - sd.kD;

    vec3 color = vec3(0.0);
    color += evalDirectionalLight(sd, global_vars.dir_light, dirlightShadowMap_1);
    return color;
}

void main() {
    vec4 position = texture(positionTex, fragTexCoord);
    vec4 normal = texture(normalTex, fragTexCoord);
    vec4 baseColor = texture(baseColorTex, fragTexCoord);
    vec4 specular = texture(specularTex, fragTexCoord);
    float depth = texture(depthTex, fragTexCoord).r;

    if(fragTexCoord.x < 0.25 && fragTexCoord.y < 0.25){
    float d = texture(dirlightShadowMap_1, fragTexCoord * 4.0).r;
    d = pow(d, 4.0);
    outColor = vec4(d,d,d,1.0);
    }else if(fragTexCoord.x < 0.5 && fragTexCoord.y < 0.25){
    float d = texture(dirlightShadowMap_2, fragTexCoord * 4.0).r;
    d = pow(d, 4.0);
    outColor = vec4(d,d,d,1.0);
    }else if(fragTexCoord.x < 0.75 && fragTexCoord.y < 0.25){
    float d = texture(dirlightShadowMap_3, fragTexCoord * 4.0).r;
    d = pow(d, 4.0);
    outColor = vec4(d,d,d,1.0);
    }else{
    vec3 color = shading(position, normal, baseColor, specular, depth);
    color = pow(color, vec3(2.2));
    outColor = vec4(color, 1.0);      

    }

    // vec3 d = global_vars.camera_pos - position.xyz;    
    // outColor = vec4(d, 1.0);
}