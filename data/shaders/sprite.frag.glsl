#version 330 core
out vec4 fragColor;

in vec2 uv;

uniform sampler2D atlas;

void main()
{
  	fragColor = texture(atlas, uv);
}
