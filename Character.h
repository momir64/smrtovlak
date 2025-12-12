#pragma once
#include "Image.h"
#include <chrono>

class Character {
	std::chrono::steady_clock::time_point lastUpdateTimeOpacity;
	std::chrono::steady_clock::time_point lastUpdateTime;
	float frameTime, xStop = 0.5f;
	long long currentFrame = 0;
	bool goLeft = true;

public:
	float position = 0.f;
	Image* idleImage;
	Image* walkImage;
	float character;
	int mode = 0;
	float walker;
	float idle;
	float sick;
	float belt;
	int idx;

	void walk(bool in, float xStop);
	void updateLayers();
	void draw();
};