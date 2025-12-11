#version 330 core
layout(location=0) in vec2 quad;

out vec2 uv;

uniform vec2 uPos;
uniform vec2 uSize;
uniform int uFlipX;
uniform float uAngle;
uniform float uPivotHeight;

void main() {
    uv = vec2(uFlipX == 1 ? 1.0 - quad.x : quad.x, quad.y);

    vec2 pivot = vec2(0.5, uPivotHeight);

    vec2 p = quad - pivot;

    float ang = radians(uAngle);
    float c = cos(ang);
    float s = sin(ang);

    p = vec2(p.x * c - p.y * s,
             p.x * s + p.y * c);

    p += pivot;

    vec2 pos;
    pos.x = (p.x - 0.5) * uSize.x + uPos.x;
    pos.y =  p.y * uSize.y + uPos.y - uPivotHeight * uSize.y;

    gl_Position = vec4(pos * 2.0 - 1.0, 0.0, 1.0);
}
