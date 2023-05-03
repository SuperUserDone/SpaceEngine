#version 330 core 

out vec4 fragColor;

in vec2 uv;
uniform sampler2D input;

void main()
{
  fragColor = vec4(texture(input, uv).rgb, 1.f);
}
