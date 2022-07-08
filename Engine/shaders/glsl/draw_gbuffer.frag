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

void main() {
    outPosition = fragWorldPos;
    vec3 fragNormal = fragTBN[2];
    outNormal = vec4(fragNormal, 1.0);
    if((mat_vars.flags & 0x00000001) != 0)
        outBaseColor = texture(baseColor, fragTexCoord);
    else
        outBaseColor = mat_vars.baseColorFactor;

    if((mat_vars.flags & 0x00000002) != 0)
        outSpecular = texture(specular, fragTexCoord);
    else
        outSpecular = mat_vars.specularFactor;
}