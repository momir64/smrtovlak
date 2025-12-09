#pragma once
#include "WindowManager.h"
#include "LineEngine.h"
#include "Image.h"

class Simulation {
	WindowManager& window;
	Image background;
	LineEngine line;

public:
	Simulation(WindowManager& window);
	void draw();
};

