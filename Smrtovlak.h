#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "WindowManager.h"
#include "InputListener.h"
#include "DataClasses.h"
#include "TextEngine.h"
#include "Simulation.h"
#include "Blueprint.h"
#include "Button.h"
#include <vector>

class Smrtovlak : public ResizeListener, public ButtonListener {
	WindowManager window;
	std::vector<PointStats> points;
	std::vector<Coords> tracks;
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

