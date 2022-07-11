#version 450

layout(set = 0, binding = 0) uniform sampler2D colorTex;

layout(location = 0) in vec2 fragCoord;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

vec3 ACESToneMapping(vec3 color)
{
    const float A = 2.51;
    const float B = 0.03;
    const float C = 2.43;
    const float D = 0.59;
    const float E = 0.14;
    return (color * (A * color + B)) / (color * (C * color + D) + E);
}

void main() {
    vec3 color = ACESToneMapping(texture(colorTex, fragTexCoord).rgb);
    color = pow(color, vec3(2.2));
    outColor = vec4(color, 1.0);
}