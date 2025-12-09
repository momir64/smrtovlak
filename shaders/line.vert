#version 330 core

layout(location=0) in vec2 aPos;
uniform vec2 uRes;

void main() {
    vec2 p = aPos / uRes;
    p = p * 2.0 - 1.0;
    p.y = -p.y;
    gl_Position = vec4(p, 0.0, 1.0);
}
