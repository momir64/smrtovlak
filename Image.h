#pragma once
#include <string>
#include <GL/glew.h>
#include "WindowManager.h"
#include "DataClasses.h"
#include "Shader.h"

class Image {
	Shader shader;
	GLuint tex = 0;
	WindowManager& window;
	int imgW = 0, imgH = 0;
	GLuint vao = 0, vbo = 0;

public:
	Image(WindowManager& window, const std::string& path);
	~Image();

	void draw(Bounds bounds);
};
