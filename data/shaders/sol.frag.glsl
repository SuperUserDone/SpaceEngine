#version 330 core
out vec4 fragColor;

in vec2 uv;

uniform sampler2D organic;

uniform float time;

uniform vec3 sunColor;

#define PI 3.14159265359

vec2 hash( vec2 p )
{
	p = vec2( dot(p,vec2(127.1,311.7)), dot(p,vec2(269.5,183.3)) );
	return -1.0 + 2.0*fract(sin(p)*43758.5453123);
}

float noise( in vec2 p )
{
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;

  	vec2  i = floor( p + (p.x+p.y)*K1 );
    vec2  a = p - i + (i.x+i.y)*K2;
    float m = step(a.y,a.x); 
    vec2  o = vec2(m,1.0-m);
    vec2  b = a - o + K2;
  	vec2  c = a - 1.0 + 2.0*K2;
    vec3  h = max( 0.5-vec3(dot(a,a), dot(b,b), dot(c,c) ), 0.0 );
  	vec3  n = h*h*h*h*vec3( dot(a,hash(i+0.0)), dot(b,hash(i+o)), dot(c,hash(i+1.0)));
    return dot( n, vec3(70.0) );
}

void main()
{
    vec2 tuv = (uv - vec2(0.5)) * 4.0f;

    float h = length(tuv) > 1.0 ? 0.0 : 1.0;
    
    vec2 starUV = normalize(tuv)*(2.0*asin(length(tuv)) / 3.1415926);

    float sTime = time * 0.033;

    float tsample = texture(organic, starUV + vec2(sTime* 1.5, 0.f)).g;
    tsample = texture(organic, starUV + vec2(tsample * 0.05 + sTime, 0.0)).r + 0.8;
    tsample *= 2.0;
    tsample *= tsample;

    vec3 color = vec3(tsample) * sunColor;
    
  	fragColor.rgb = h*color;
    fragColor.a = 1.f;
}
