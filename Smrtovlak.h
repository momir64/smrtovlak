#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "WindowManager.h"
#include "InputListener.h"
#include "TextEngine.h"
#include "Button.h"
#include "Image.h"
#include <iostream>

class Smrtovlak : public ResizeListener, public ButtonListener {
	WindowManager window;
	TextEngine signature;
	Image background;
	Button button;

public:
	Smrtovlak();
	void resizeCallback(GLFWwindow& window);
	void draw();
	int run();

	void buttonChanged(Button& btn, int newMode) override {
		std::cout << "Button mode = " << newMode << "\n";
	}
};

