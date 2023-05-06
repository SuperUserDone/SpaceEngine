#version 330 core

// This shader performs downsampling on a texture,
// as taken from Call Of Duty method, presented at ACM Siggraph 2014.
// This particular method was customly designed to eliminate
// "pulsating artifacts and temporal stability issues".

uniform sampler2D srcTexture;
uniform vec2 srcResolution;
uniform bool firstpass;
uniform vec4 bloomParams; 

in vec2 uv;
layout (location = 0) out vec4 downsample;

vec3 threshold(vec3 color, float threshold, vec3 curve)
{
    float brightness = max(max(color.x, color.y), color.z);
    float rq = clamp(brightness - curve.x, 0.0, curve.y);
    rq = (rq * rq) * curve.z;
    color *= (max(rq, brightness - threshold) / max(brightness, 9.9999997473787516355514526367188e-05));
    return color;
}

vec3 prefilter(vec3 color)
{
    float clampValue = 20.0;
    color = clamp(color, vec3(0.0), vec3(clampValue));
    return threshold(color, bloomParams.x, bloomParams.yzw);
}

void main()
{
    vec2 srcTexelSize = 1.0 / srcResolution;
    float x = srcTexelSize.x;
    float y = srcTexelSize.y;

    // Take 13 samples around current texel:
    // a - b - c
    // - j - k -
    // d - e - f
    // - l - m -
    // g - h - i
    // === ('e' is the current texel) ===
    vec3 a = texture(srcTexture, vec2(uv.x - 2*x, uv.y + 2*y)).rgb;
    vec3 b = texture(srcTexture, vec2(uv.x,       uv.y + 2*y)).rgb;
    vec3 c = texture(srcTexture, vec2(uv.x + 2*x, uv.y + 2*y)).rgb;

    vec3 d = texture(srcTexture, vec2(uv.x - 2*x, uv.y)).rgb;
    vec3 e = texture(srcTexture, vec2(uv.x,       uv.y)).rgb;
    vec3 f = texture(srcTexture, vec2(uv.x + 2*x, uv.y)).rgb;

    vec3 g = texture(srcTexture, vec2(uv.x - 2*x, uv.y - 2*y)).rgb;
    vec3 h = texture(srcTexture, vec2(uv.x,       uv.y - 2*y)).rgb;
    vec3 i = texture(srcTexture, vec2(uv.x + 2*x, uv.y - 2*y)).rgb;

    vec3 j = texture(srcTexture, vec2(uv.x - x, uv.y + y)).rgb;
    vec3 k = texture(srcTexture, vec2(uv.x + x, uv.y + y)).rgb;
    vec3 l = texture(srcTexture, vec2(uv.x - x, uv.y - y)).rgb;
    vec3 m = texture(srcTexture, vec2(uv.x + x, uv.y - y)).rgb;

    // Apply weighted distribution:
    // 0.5 + 0.125 + 0.125 + 0.125 + 0.125 = 1
    // a,b,d,e * 0.125
    // b,c,e,f * 0.125
    // d,e,g,h * 0.125
    // e,f,h,i * 0.125
    // j,k,l,m * 0.5
    // This shows 5 square areas that are being sampled. But some of them overlap,
    // so to have an energy preserving downsample we need to make some adjustments.
    // The weights are the distributed, so that the sum of j,k,l,m (e.g.)
    // contribute 0.5 to the final color output. The code below is written
    // to effectively yield this sum. We get:
    // 0.125*5 + 0.03125*4 + 0.0625*4 = 1
    downsample.rgb = e*0.125;
    downsample.rgb += (a+c+g+i)*0.03125;
    downsample.rgb += (b+d+f+h)*0.0625;
    downsample.rgb += (j+k+l+m)*0.125;
    if(firstpass)
      downsample.rgb = prefilter(downsample.rgb);

    downsample.a = 1.f;
  }

