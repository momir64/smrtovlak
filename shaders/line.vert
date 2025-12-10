#version 330 core
layout(location=0) in vec2 aPos;
layout(location=1) in float aSide;
layout(location=2) in float aProj;

uniform vec2 uRes;

out float vSide;
out float vProj;

void main() {
    vSide = aSide;
    vProj = aProj;
    vec2 p = aPos / uRes;
    p = p * 2.0 - 1.0;
    p.y = -p.y;
    gl_Position = vec4(p, 0.0, 1.0);
}
