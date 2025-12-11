#pragma once
#include "WindowManager.h"
#include "InputListener.h"
#include "DataClasses.h"
#include "GLFW/glfw3.h"
#include "LineEngine.h"
#include "Image.h"
#include <vector>
#include <string>
#include <chrono>

class Simulation;

class Character {
public:
	float character;
	float sick;
	float belt;
};

class Train : public KeyboardListener, MouseListener {
	float currentDistance, stopDistance, currentSpeed = 0.f, preStopSpeed = 0.f, sleep = 0.f;
	std::chrono::steady_clock::time_point lastUpdateTime;
	std::vector<Character> characters;
	std::vector<PointStats>& points;
	std::vector<Coords>& tracks;
	float BOTTOM_RATIO, MARGIN;
	Simulation& simulation;
	WindowManager& window;
	LineEngine lineEngine;
	int noCars, mode = 0;
	Image* carImage;

	std::vector<float> getLayers(int carIdx);
	Bounds toContent(Bounds bounds) const;
	Coords toRelative(Coords point) const;
	Coords toScreen(Coords point) const;
	PointStats closestPoint(float dist);
	int seatHit(float x, float y);
	void updateDistance();
	void reset();

public:
	Train(WindowManager& window, Simulation& simulation, std::vector<Coords>& tracks, std::vector<PointStats>& points,
		float bottomRatio, float margin, int noCars, const std::vector<std::string>& characters);
	void keyboardCallback(GLFWwindow& window, int key, int scancode, int action, int mods) override;
	void mouseCallback(double x, double y, int button, int action, int mods) override;

	void draw();
};

