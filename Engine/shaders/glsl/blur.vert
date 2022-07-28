#version 450

layout(location = 0) out vec2 fragCoord;
layout(location = 1) out vec2 fragTexCoord;

vec2 positions[3] = vec2[](
    vec2(-1.0, -1.0),
    vec2(-1.0, 3.0),
    vec2(3.0, -1.0)
);

vec2 texcoord[3] = vec2[](
    vec2(0.0, 0.0),
    vec2(0.0, 2.0),
    vec2(2.0, 0.0)
);

void main() {
    fragCoord = positions[gl_VertexIndex];
    fragTexCoord = texcoord[gl_VertexIndex];
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}