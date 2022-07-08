#version 450

layout(set = 0, binding = 0) uniform GlobalUniforms {
    mat4 proj_view;
    vec3 camera_pos;
} global_vars;

layout(set = 1, binding = 0) uniform PerDrawUniforms {
    mat4 model;
    mat4 normal_mat;
} per_draw_vers;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBitangent;

layout(location = 0) out vec4 fragWorldPos;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out mat3 fragTBN;

void main() {
    fragWorldPos = per_draw_vers.model * vec4(inPosition, 1.0);
    gl_Position = global_vars.proj_view * fragWorldPos;
    
    mat3 normal_mat = mat3(per_draw_vers.normal_mat);
    fragTBN = normal_mat * mat3(inTangent, inBitangent, inNormal);
    fragTBN[0] = normalize(fragTBN[0]);
    fragTBN[1] = normalize(fragTBN[1]);
    fragTBN[2] = normalize(fragTBN[2]);

    fragTexCoord = inTexCoord;
}