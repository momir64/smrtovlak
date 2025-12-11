#version 330 core
in vec2 uv;
out vec4 col;

uniform sampler2D tex;
uniform float uAlpha;

void main() {
    col = texture(tex, uv) * uAlpha;
}
