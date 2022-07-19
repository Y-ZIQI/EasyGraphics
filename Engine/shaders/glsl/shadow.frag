#version 450

#ifdef SHADOW_SOFT_EVSM

layout(location = 0) out vec4 outDepth;

const float posi_c = 40.0;
const float nega_c = 40.0;

void main(){
    outDepth.r = exp(gl_FragCoord.z * posi_c);
    outDepth.g = exp(gl_FragCoord.z * posi_c * 2.0);
    outDepth.b = -exp(-gl_FragCoord.z * nega_c);
    outDepth.a = exp(-gl_FragCoord.z * nega_c * 2.0);
}

#else

layout(location = 0) out float outDepth;

void main()
{
    outDepth = gl_FragCoord.z;
}

#endif
