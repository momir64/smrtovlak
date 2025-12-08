#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "WindowManager.h"
#include "InputListener.h"
#include "TextEngine.h"
#include "Image.h"

class Smrtovlak : ResizeListener {
	WindowManager window;
	TextEngine signature;
	Image background;

public:
	Smrtovlak();
	void resizeCallback(GLFWwindow& window);
	void draw();
	int run();
};

