#version 330 core 

out vec4 fragColor;

in vec2 uv;
uniform sampler2D screenTex;

void main()
{
  fragColor = vec4(texture(screenTex, uv).rgb, 1.f);
}
