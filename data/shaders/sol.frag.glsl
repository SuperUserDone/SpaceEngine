#version 330 core
out vec4 FragColor;

in vec2 uv;

uniform float time;
const float PI = 3.14159265359;
void main() {
  FragColor = vec4(vec3((sin(time*2*PI) + 1) / 2.f), 1.f);
}

