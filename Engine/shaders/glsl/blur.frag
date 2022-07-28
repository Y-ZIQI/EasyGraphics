#version 450

layout(set = 0, binding = 0) uniform sampler2D colorTex;

layout(location = 0) in vec2 fragCoord;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

vec3 blur3x3(sampler2D src_texture, vec2 uv){
    return 
        (textureOffset(src_texture, uv, ivec2(-1, -1)).rgb + 
        textureOffset(src_texture, uv, ivec2(-1, 0)).rgb + 
        textureOffset(src_texture, uv, ivec2(-1, 1)).rgb + 
        textureOffset(src_texture, uv, ivec2(0, -1)).rgb + 
        textureOffset(src_texture, uv, ivec2(0, 0)).rgb + 
        textureOffset(src_texture, uv, ivec2(0, 1)).rgb + 
        textureOffset(src_texture, uv, ivec2(1, -1)).rgb + 
        textureOffset(src_texture, uv, ivec2(1, 0)).rgb + 
        textureOffset(src_texture, uv, ivec2(1, 1)).rgb) * (1.0 / 9.0)
    ;
}

void main() {
    vec3 color = blur3x3(colorTex, fragTexCoord);
    // vec3 color = texture(colorTex, fragTexCoord).rgb;
    // color = pow(color, vec3(2.2));
    outColor = vec4(color, 1.0);
}