#pragma once
#include "WindowManager.h"
#include "Image.h"

class Simulation {
	Image background;

public:
	Simulation(WindowManager& window);
	void draw();
};

