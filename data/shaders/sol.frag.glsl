#version 330 core
out vec4 fragColor;

in vec2 uv;

uniform sampler2D organic;
uniform float time;
uniform vec3 sunColor;
uniform float raduis;

#define PI 3.14159265359

void main()
{
    vec2 tuv = (uv - vec2(0.5)) * 2.0f;

    float h = 1.0 - smoothstep(1.f - 3.f/raduis, 1.0f, length(tuv)); 
    
    vec2 starUV = normalize(tuv)*(2.0*asin(length(tuv)) / PI);

    float sTime = time * 0.05;

    float tsample = texture(organic, starUV + vec2(sTime* 1.5, 0.f)).g;
    tsample = mix(texture(organic, starUV + vec2(tsample * 0.1 + sTime, 0.0)).r, 1.0f, 0.7f);
    tsample *= 4.0f;

    vec3 color = vec3(tsample) * sunColor;
  	fragColor.rgb = color;

    fragColor.a = h;
}
