#pragma once
#include "WindowManager.h"
#include "DataClasses.h"
#include "LineEngine.h"
#include "Image.h"
#include <vector>

class Simulation {
	std::vector<float> randomLevelOffsets;
	std::vector<Coords>& tracks;
	Image background, platform;
	WindowManager& window;
	LineEngine lineEngine;

	Bounds trackBounds() const;

	void drawLine(std::vector<Coords> line, const Color& color, float width, float gradient, float gradientAngle, float pixelSize);
	void drawScaffolding();

public:
	Simulation(WindowManager& window, std::vector<Coords>& tracks);
	void draw();
};

