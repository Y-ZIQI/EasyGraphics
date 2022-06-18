#version 450

layout(set = 1, binding = 0) uniform _unused_name_permaterial
{
    vec4  baseColorFactor;
    // highp float metallicFactor;
    // highp float roughnessFactor;
    // highp float normalScale;
    // highp float occlusionStrength;
    // highp vec3  emissiveFactor;
    // uint        is_blend;
    // uint        is_double_sided;
};

layout(set = 1, binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(texSampler, fragTexCoord);
    //outColor = vec4(0.5 * outColor.rgb + vec3(0.5, 0.0, 0.0), 1.0);
}