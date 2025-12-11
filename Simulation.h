#pragma once
#include "WindowManager.h"
#include "DataClasses.h"
#include "LineEngine.h"
#include "Image.h"
#include "Train.h"
#include <vector>

class Simulation {
	std::vector<float> randomLevelOffsets;
	std::vector<Coords>& tracks;
	Image background, platform;
	WindowManager& window;
	LineEngine lineEngine;
	Train train;

	void drawScaffolding();

public:
	Simulation(WindowManager& window, std::vector<Coords>& tracks, std::vector<PointStats>& points);
	void drawLine(std::vector<Coords> line, const Color& color, float width, float gradient, float gradientAngle, float pixelSize, bool includeBottom = true);
	Bounds trackBounds() const;
	void draw();
};

