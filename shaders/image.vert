#version 330 core
layout(location=0) in vec2 quad;

out vec2 uv;

uniform vec2 uPos;
uniform vec2 uSize;

void main() {
    vec2 p;
    uv = quad;
    p.x = (quad.x - 0.5) * uSize.x + uPos.x;
    p.y =  quad.y        * uSize.y + uPos.y;
    gl_Position = vec4(p * 2.0 - 1.0, 0.0, 1.0);
}
