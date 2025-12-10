#include "Simulation.h"
#include "WindowManager.h"
#include "DataClasses.h"
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <thread>
#include <random>

namespace {
	constexpr float MARGIN = 14.f, TOP = 110.f, BOTTOM_RATIO = 0.034f;
	constexpr float PLATFORM_WIDTH = 0.42f;

	constexpr float POLE_DISTANCE = 0.08f, MIN_POLE_DISTANCE = 0.02f;
	constexpr float FLOOR_Y = 0.0005f, FLOOR_Y_MAX = 0.03f;
	constexpr float DIMM_FACTOR = 0.6f, POLE_WIDTH = 5.2f;
	constexpr int POLE_BUFFER = 32, LAST_POLE_IDX = 0;

	constexpr int MAX_LEVELS = 128;
}

Bounds Simulation::trackBounds() const {
	return Bounds(MARGIN, TOP,
		window.getWidth() - MARGIN * 2.f,
		window.getHeight() - BOTTOM_RATIO * window.getWidth() - TOP);
}

void Simulation::drawLine(std::vector<Coords> line, const Color& color, float width, float gradient, float gradientAngle, float pixelSize) {
	if (line.size() < 2) return;
	Bounds b = trackBounds();
	float unit = b.width;

	std::vector<Coords> points;
	points.reserve(line.size());
	for (auto& p : line) {
		float x = b.x + p.x * b.width;
		float y = window.getHeight() - (p.y * unit) - BOTTOM_RATIO * window.getWidth();
		points.emplace_back(x, y);
	}

	width *= window.getWidth() / 1000.f;
	lineEngine.draw(points, color, width, gradient, gradientAngle, pixelSize);
}

static inline Color dimmColor(const Color& color, float dimFactor, float depth) {
	if (depth <= 0.0f) return color;

	if (depth >= 1.0f) {
		float dr = color.red * dimFactor;
		float dg = color.green * dimFactor;
		float db = color.blue * dimFactor;
		return Color(dr, dg, db);
	}

	float dr = color.red * dimFactor;
	float dg = color.green * dimFactor;
	float db = color.blue * dimFactor;

	float r = color.red * (1.0f - depth) + dr * depth;
	float g = color.green * (1.0f - depth) + dg * depth;
	float b = color.blue * (1.0f - depth) + db * depth;

	return Color(r, g, b);
}

void Simulation::drawScaffolding() {
	std::vector<std::vector<Coords>> poleRootLevels = { { tracks[0] } };
	Color poleColor(153, 158, 160);
	bool leftDirection = true;
	int lastPoleIdx = 0;

	for (int i = POLE_BUFFER; i < tracks.size(); i++) {
		if ((tracks[i].x > tracks[i - POLE_BUFFER].x) == leftDirection) {
			int start = i - POLE_BUFFER, extremeIdx = start;
			float extremeX = tracks[start].x;

			for (int k = start + 1; k <= i; k++) {
				float x = tracks[k].x;
				if (leftDirection ? (x < extremeX) : (x > extremeX)) {
					extremeIdx = k;
					extremeX = x;
				}
			}

			if (abs(poleRootLevels.back().back().x - tracks[extremeIdx].x) < MIN_POLE_DISTANCE)
				poleRootLevels.back().pop_back();

			poleRootLevels.back().push_back({ tracks[extremeIdx] });
			poleRootLevels.push_back({ tracks[extremeIdx] });
			leftDirection = !leftDirection;
			lastPoleIdx = start;
		}

		float randomLevelOffset = randomLevelOffsets[std::min(MAX_LEVELS - 1, int(poleRootLevels.size()))];
		if (abs(tracks[lastPoleIdx].x - tracks[i].x) - randomLevelOffset > POLE_DISTANCE) {
			poleRootLevels.back().push_back(tracks[i]);
			lastPoleIdx = i;
		}
	}

	int n = poleRootLevels.size();
	int mid = n / 2;

	for (int o = 0; o < n; o++) {
		int level = (o % 2 == 0) ? (mid + o / 2) : (mid - 1 - o / 2);
		if (level < 0 || level >= n) continue;

		for (int pole = poleRootLevels[level].size() - 2; pole >= 0; pole--) {
			if (level == 0 && poleRootLevels[level][pole].x > 0.5 - PLATFORM_WIDTH / 2) continue;
			if (level == poleRootLevels.size() - 1 && poleRootLevels[level][pole].x < 0.5 + PLATFORM_WIDTH / 2) continue;

			float depth = 1 - std::abs(float(level - mid) / float(mid));
			Coords poleRoot = poleRootLevels[level][pole];
			Coords poleFloor(poleRoot.x, FLOOR_Y + depth * (FLOOR_Y_MAX - FLOOR_Y));
			drawLine({ poleRoot, poleFloor }, dimmColor(poleColor, DIMM_FACTOR, depth), POLE_WIDTH, 0.1f, 0.f, POLE_WIDTH * 0.45);
		}
	}
}

Simulation::Simulation(WindowManager& window, std::vector<Coords>& tracks) :
	background(window, "assets/background.png"),
	platform(window, "assets/platform.png"),
	lineEngine(window), window(window), tracks(tracks) {

	std::random_device rd;
	std::default_random_engine eng(rd());
	std::uniform_real_distribution<float> dist(0, 0.5);
	for (int i = 0; i < MAX_LEVELS; i++)
		randomLevelOffsets.push_back(POLE_DISTANCE * dist(eng));
}

void Simulation::draw() {
	background.draw(Bounds(0.5f, 0.f, 1.f));
	drawScaffolding();
	platform.draw(Bounds(0.5f, BOTTOM_RATIO, PLATFORM_WIDTH));
	drawLine(tracks, Color(171, 87, 65), 5.2f, 0.2f, -80.f, 5.2f * 0.32);
}
