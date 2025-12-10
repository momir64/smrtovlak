#pragma once
#include <vector>
#include <GL/glew.h>
#include "Shader.h"
#include "WindowManager.h"
#include "DataClasses.h"

class LineEngine {
	GLuint vao = 0, vbo = 0;
	WindowManager& window;
	Shader shader;

public:
	LineEngine(WindowManager& window);
	void draw(const std::vector<Coords>& points, const Color& color, float width, float brightAmount = 0.f, float angleDeg = 0.f, float pixelSize = 0.0f);
};
