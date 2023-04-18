#version 330 core
layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 iuv;

out vec2 uv;

uniform mat4 transform;

void main() {
  gl_Position = transform * vec4(pos.x, pos.y, 0.0, 1.0);
  uv = iuv;
}
