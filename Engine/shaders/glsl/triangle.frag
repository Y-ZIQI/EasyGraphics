#version 450

layout(set = 2, binding = 0) uniform _unused_name_permaterial
{
    uint flags;
    vec4 baseColorFactor;
    // highp float metallicFactor;
    // highp float roughnessFactor;
    // highp float normalScale;
    // highp float occlusionStrength;
    // highp vec3  emissiveFactor;
    // uint        is_blend;
    // uint        is_double_sided;
}mat_vars;

layout(set = 2, binding = 1) uniform sampler2D baseColor;
layout(set = 2, binding = 2) uniform sampler2D specular;
layout(set = 2, binding = 3) uniform sampler2D normal;
layout(set = 2, binding = 4) uniform sampler2D emissive;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    if((mat_vars.flags & 0x00000001) != 0)
        outColor = texture(baseColor, fragTexCoord);
    else
        outColor = mat_vars.baseColorFactor;
    //outColor = vec4(0.5 * outColor.rgb + vec3(0.5, 0.0, 0.0), 1.0);
}