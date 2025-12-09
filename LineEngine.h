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

	void extendSegment(const Coords& A, const Coords& B, float width, Coords& A2, Coords& B2);

public:
	LineEngine(WindowManager& window);
	void draw(const std::vector<Coords>& points, const Color& color, float width);
};