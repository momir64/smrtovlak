#include "DataClasses.h"
#include "Character.h"
#include <algorithm>
#include <random>
#include <chrono>
#include <cmath>

namespace {
	constexpr float PLATFORM_WIDTH_IN = 0.42f, PLATFORM_WIDTH_OUT = 0.45f, CHARACTER_WIDTH = 0.1f;
	constexpr float GROUND_HEIGHT = 0.035f, PLATFORM_HEIGHT = 0.068f;
	constexpr float APPEARANCE_SPEED = 0.1f, START_OPACITY = 0.08f;
	constexpr float FPS = 14.f, MOVING_SPEED = 0.09f;
	constexpr float IDLE_TIME = 0.24f;
}

void Character::walk(bool in, float xStop) {
	std::mt19937 rng(std::random_device{}());
	std::bernoulli_distribution dist(0.5);
	walker = in ? 1.f : START_OPACITY;
	this->xStop = xStop;
	goLeft = dist(rng);
	mode = in ? 1 : 2;
	idle = 0.f;

	if (in && goLeft)
		position = 1.f + CHARACTER_WIDTH / 2.f;
	else if (in && !goLeft)
		position = -CHARACTER_WIDTH / 2.f;
	else
		position = xStop;
}


void Character::draw() {
	float dt;
	int tryAgain = 2;
	while (tryAgain > 0) {
		auto now = std::chrono::steady_clock::now();
		dt = std::chrono::duration<float>(now - lastUpdateTime).count();
		lastUpdateTime = now;
		if (dt < 0.5) tryAgain--;
		tryAgain--;
	}

	if (idle > 0.f) {
		if (frameTime < 0.f) {
			idle = -1.f;
			character = START_OPACITY;
			mode = 0;
		} else
			frameTime -= dt;
	}

	if (std::fabs(idle) > 0.f) {
		idleImage->draw(Bounds(xStop, PLATFORM_HEIGHT, CHARACTER_WIDTH), { std::fabs(idle) }, goLeft);
		return;
	}

	if (mode == 0) return;

	if (goLeft)
		position -= MOVING_SPEED * dt;
	else
		position += MOVING_SPEED * dt;

	float stairs = (PLATFORM_WIDTH_IN / 2.f - std::fabs(position - 0.5f)) / (PLATFORM_WIDTH_OUT - PLATFORM_WIDTH_IN);
	stairs = std::clamp(stairs, 0.0f, 1.0f);
	float y = GROUND_HEIGHT + (PLATFORM_HEIGHT - GROUND_HEIGHT) * stairs;

	if (mode == 1 && ((goLeft && position < xStop) || (!goLeft && position > xStop))) {
		frameTime = IDLE_TIME;
		walker = 0.f;
		idle = 1.f;
		return;
	}

	if (mode == 2 && (position < 0.f || position > 1.f)) {
		character = 0.f;
		walker = 0.f;
		sick = 0.f;
		mode = 0;
	}

	walkImage->draw(Bounds(position, y, CHARACTER_WIDTH), { std::fabs(walker) }, goLeft, 0.f, 0.f, true, currentFrame);
	frameTime += dt;
	if (frameTime >= 1.f / FPS) {
		frameTime = 0.f;
		currentFrame++;
	}
}

void Character::updateLayers() {
	float dt;
	int tryAgain = 2;
	while (tryAgain > 0) {
		auto now = std::chrono::steady_clock::now();
		dt = std::chrono::duration<float>(now - lastUpdateTimeOpacity).count();
		lastUpdateTimeOpacity = now;
		if (dt < 0.5) tryAgain--;
		tryAgain--;
	}

	float delta = APPEARANCE_SPEED * dt * 60.f;

	character = std::min(1.f, character + delta);
	sick = std::min(1.f, sick + delta);
	belt = std::min(1.f, belt + delta);
	walker = std::min(1.f, walker + delta);
	idle = std::min(1.f, idle + delta);

	if (std::fabs(character) <= delta * 1.5f) character = 0.f;
	if (std::fabs(sick) <= delta * 1.5f) sick = 0.f;
	if (std::fabs(belt) <= delta * 1.5f) belt = 0.f;
	if (std::fabs(walker) <= delta * 1.5f) walker = 0.f;
	if (std::fabs(idle) <= delta * 1.5f) idle = 0.f;
}