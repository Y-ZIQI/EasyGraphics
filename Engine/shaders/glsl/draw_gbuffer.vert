#version 450

layout(set = 0, binding = 0) uniform GlobalUniforms {
    mat4 proj_view;
    vec3 camera_pos;
} global_vars;

layout(set = 1, binding = 0) uniform PerDrawUniforms {
    mat4 model;
    mat3 normal_mat;
} per_draw_vers;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec4 fragWorldPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTexCoord;

void main() {
    fragWorldPos = per_draw_vers.model * vec4(inPosition, 1.0);
    gl_Position = global_vars.proj_view * fragWorldPos;
    
    fragNormal = per_draw_vers.normal_mat * inNormal;
    fragTexCoord = inTexCoord;
}