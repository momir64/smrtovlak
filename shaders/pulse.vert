#version 330 core
layout(location=0) in vec2 aPos;
out vec2 uv;
void main() {
    uv = aPos;
    vec2 p = aPos;
    p = p * 2.0 - 1.0;
    p.y = -p.y;
    gl_Position = vec4(p, 0.0, 1.0);
}
