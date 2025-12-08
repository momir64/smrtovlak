#version 330 core
layout(location=0) in vec2 aPos;
out vec2 uv;

uniform vec2 uPos;
uniform vec2 uSize;

void main() {
    uv = aPos;
    vec2 p = uPos + aPos * uSize;
    gl_Position = vec4(p * 2.0 - 1.0, 0, 1);
}
