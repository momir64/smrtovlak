#version 330 core

in vec2 uv;
out vec4 col;

uniform vec3 uBg;
uniform vec3 uEdge;
uniform float uPix;

void main() {
    vec2 block = floor(uv * uPix);
    vec2 centerUV = (block + 0.5) / uPix;

    float r = 0.65;
    float border = 0.12;

    vec2 c = centerUV * 2.0 - 1.0;
    float d = length(max(abs(c) - vec2(1.0 - r), 0.0));

    if (d > r)
        discard;

    bool isEdge = d > r - border;

    vec3 color = isEdge ? uEdge : uBg;
    col = vec4(color, 1.0);
}
