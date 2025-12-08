#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "WindowManager.h"
#include "InputListener.h"
#include "TextEngine.h"
#include "Simulation.h"
#include "Blueprint.h"
#include "Button.h"

class Smrtovlak : public ResizeListener, public ButtonListener {
	WindowManager window;
	Simulation simulation;
	TextEngine signature;
	Blueprint blueprint;
	Button button;

public:
	Smrtovlak();
	void resizeCallback(GLFWwindow& window);
	void draw();
	int run();

	void buttonChanged(Button& button, int selected) override;
};

