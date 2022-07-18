#version 450

layout(set = 0, binding = 0) uniform GlobalUniforms {
    mat4 proj_view;
} global_vars;

layout(set = 1, binding = 0) uniform PerDrawUniforms {
    mat4 model;
} per_draw_vers;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBitangent;

void main() {
    gl_Position = global_vars.proj_view * per_draw_vers.model * vec4(inPosition, 1.0);
}