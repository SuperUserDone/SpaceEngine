#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 3) in vec2 iuv;

out vec2 uv;

uniform mat4 transform;

void main() {
  gl_Position = transform * vec4(pos, 1.0);
  uv = iuv;
}