#version 330 core
out vec4 FragColor;

in vec2 uv;

uniform vec3 color;

void main() {
  FragColor = vec4(color.rgb, 1.f);
}

