#version 330 core

// This shader performs upsampling on a texture,
// as taken from Call Of Duty method, presented at ACM Siggraph 2014.

uniform sampler2D srcTexture;
uniform float filterRadius;

in vec2 uv;
layout (location = 0) out vec4 upsample;

void main()
{
    // The filter kernel is applied with a radius, specified in texture
    // coordinates, so that the radius will vary across mip resolutions.
    float x = filterRadius;
    float y = filterRadius;

    // Take 9 samples around current texel:
    // a - b - c
    // d - e - f
    // g - h - i
    // === ('e' is the current texel) ===
    vec3 a = texture(srcTexture, vec2(uv.x - x, uv.y + y)).rgb;
    vec3 b = texture(srcTexture, vec2(uv.x,     uv.y + y)).rgb;
    vec3 c = texture(srcTexture, vec2(uv.x + x, uv.y + y)).rgb;

    vec3 d = texture(srcTexture, vec2(uv.x - x, uv.y)).rgb;
    vec3 e = texture(srcTexture, vec2(uv.x,     uv.y)).rgb;
    vec3 f = texture(srcTexture, vec2(uv.x + x, uv.y)).rgb;

    vec3 g = texture(srcTexture, vec2(uv.x - x, uv.y - y)).rgb;
    vec3 h = texture(srcTexture, vec2(uv.x,     uv.y - y)).rgb;
    vec3 i = texture(srcTexture, vec2(uv.x + x, uv.y - y)).rgb;

    // Apply weighted distribution, by using a 3x3 tent filter:
    //  1   | 1 2 1 |
    // -- * | 2 4 2 |
    // 16   | 1 2 1 |
    upsample.rgb = e*4.0;
    upsample.rgb += (b+d+f+h)*2.0;
    upsample.rgb += (a+c+g+i);
    upsample.rgb *= 1.0 / 16.0;
    upsample.a = 1.f;
}
