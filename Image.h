#pragma once
#include "WindowManager.h"
#include "DataClasses.h"
#include <GL/glew.h>
#include "Shader.h"
#include <string>
#include <vector>

class Image {
	Shader shader;
	std::vector<GLuint> textures;
	WindowManager& window;
	int imgW = 0, imgH = 0;
	GLuint vao = 0, vbo = 0;

public:
	Image(WindowManager& window, const std::vector<std::string>& paths);
	~Image();

	void draw(Bounds bounds, const std::vector<float>& opacity, bool flipX = false, float angle = 0.f, float pivotHeight = 0.f, bool widthRelative = true, long long frame = -1);
};
