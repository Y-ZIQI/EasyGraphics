#version 450

layout(set = 0, binding = 0) readonly buffer GlobalUniforms
{
    mat4 proj_view;
}global_vars;

// layout(set = 1, binding = 0) uniform _unused_name_permaterial
// {
//     uint flags;
//     vec4 baseColorFactor;
//     // highp float metallicFactor;
//     // highp float roughnessFactor;
//     // highp float normalScale;
//     // highp float occlusionStrength;
//     // highp vec3  emissiveFactor;
//     // uint        is_blend;
//     // uint        is_double_sided;
// }gbuffer_vars;

layout(set = 1, binding = 0) uniform sampler2D positionTex;
layout(set = 1, binding = 1) uniform sampler2D normalTex;
layout(set = 1, binding = 2) uniform sampler2D baseColorTex;
layout(set = 1, binding = 3) uniform sampler2D specularTex;
layout(set = 1, binding = 4) uniform sampler2D depthTex;

layout(location = 0) in vec2 fragCoord;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 position = texture(positionTex, fragTexCoord);
    vec4 normal = texture(normalTex, fragTexCoord);
    vec4 baseColor = texture(baseColorTex, fragTexCoord);
    vec4 specular = texture(specularTex, fragTexCoord);
    outColor = vec4(position.r, normal.g, baseColor.b, 1.0);
    outColor = vec4(1.0, 0.0, 0.0, 1.0);
}