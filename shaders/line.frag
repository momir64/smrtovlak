#version 330 core

out vec4 col;
uniform vec3 uColor;

void main() { 
	col = vec4(uColor, 1.0);
}
