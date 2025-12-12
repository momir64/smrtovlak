#include "Simulation.h"
#include "WindowManager.h"
#include "DataClasses.h"
#include <algorithm>
#include <utility>
#include <cstdlib>
#include <vector>
#include <random>
#include <string>

namespace {
	constexpr float MARGIN = 14.f, BOTTOM_RATIO = 0.034f;
	constexpr float PLATFORM_WIDTH = 0.42f;

	constexpr float POLE_DISTANCE = 0.028f, MIN_POLE_DISTANCE = 0.003f;
	constexpr float FLOOR_Y = 0.0005f, FLOOR_Y_MAX = 0.03f;
	constexpr float DIMM_FACTOR = 0.58f, POLE_WIDTH = 5.2f;
	constexpr int POLE_BUFFER = 32, LAST_POLE_IDX = 0;
	constexpr float SCAFFOLD_POLE_PERCENT = 0.72f;

	constexpr int MAX_LEVELS = 128;

	static inline std::vector<std::string> shuffledCharacters() {
		std::vector<std::string> characters = { "actor", "gangster", "gentleman" , "hobo" , "irishman" , "man" , "woman" , "worker" };
		std::random_device rd;
		std::mt19937 rng(rd());
		std::shuffle(characters.begin(), characters.end(), rng);
		return characters;
	}
	std::vector<std::string> CHARACTERS = shuffledCharacters();
}

Bounds Simulation::trackBounds() const {
	return Bounds(MARGIN, 0, window.getWidth() - MARGIN * 2.f, 0);
}

void Simulation::drawLine(std::vector<Coords> line, const Color& color, float width, float gradient, float gradientAngle, float pixelSize, bool includeBottom) {
	if (line.size() < 2) return;
	Bounds b = trackBounds();
	float unit = b.width;

	std::vector<Coords> points;
	points.reserve(line.size());
	for (auto& p : line) {
		float x = b.x + p.x * b.width;
		float y = window.getHeight() - (p.y * unit);
		if (includeBottom)
			y -= BOTTOM_RATIO * window.getWidth();
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

static inline float findClosestTrackX(float y, int rootA, int rootB, const std::vector<Coords>& tracks, float referenceX) {
	float bestX = referenceX;
	float bestDist = 1e9f;

	for (int t = rootA; t <= rootB; t++) {
		const Coords& a = tracks[t];
		const Coords& b = tracks[t + 1];

		if ((y >= std::min(a.y, b.y)) && (y <= std::max(a.y, b.y))) {
			float dy = b.y - a.y;
			if (std::abs(dy) < 1e-6f) continue;

			float tpos = (y - a.y) / dy;
			float xAtY = a.x + tpos * (b.x - a.x);
			float dist = std::abs(xAtY - referenceX);

			if (dist < bestDist) {
				bestDist = dist;
				bestX = xAtY;
			}
		}
	}
	return bestX;
}

void Simulation::drawScaffolding() {
	std::vector<std::vector<std::pair<Coords, int>>> poleRootLevels = { { {tracks[0], 0} } };
	int lastPoleIdx = 0, exLeftPoleIdx = 0, exRightPoleIdx = 0;
	Color poleColor(153, 158, 160);
	float randomLevelOffset = 0;
	bool leftDirection = true;

	for (int i = 1; i < tracks.size(); i++) {
		if (tracks[i].x < tracks[exLeftPoleIdx].x) exLeftPoleIdx = i;
		if (tracks[i].x > tracks[exRightPoleIdx].x) exRightPoleIdx = i;
	}

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

			if (abs(poleRootLevels.back().back().first.x - tracks[extremeIdx].x) < MIN_POLE_DISTANCE)
				poleRootLevels.back().pop_back();

			randomLevelOffset = randomLevelOffsets[std::min(MAX_LEVELS - 1, int(poleRootLevels.size()))];
			poleRootLevels.back().push_back({ std::pair(tracks[extremeIdx], extremeIdx) });
			poleRootLevels.push_back({ std::pair(tracks[extremeIdx], extremeIdx) });
			leftDirection = !leftDirection;
			lastPoleIdx = start;
		}

		if (abs(tracks[lastPoleIdx].x - tracks[i].x) - randomLevelOffset > POLE_DISTANCE
			&& !(leftDirection && i > exLeftPoleIdx && i < exRightPoleIdx)) {
			poleRootLevels.back().push_back(std::pair(tracks[i], i));
			randomLevelOffset = 0;
			lastPoleIdx = i;
		}
	}

	std::vector<int> unique;
	unique.reserve(tracks.size());
	int lastIdx = -1;

	for (auto& lvl : poleRootLevels)
		for (auto& p : lvl)
			if (p.second != lastIdx) {
				unique.push_back(p.second);
				lastIdx = p.second;
			}

	int totalPoles = unique.size();
	int globalMid = totalPoles / 2;

	std::vector<int> globalIndexOfTrack(tracks.size(), -1);
	for (int i = 0; i < totalPoles; i++)
		globalIndexOfTrack[unique[i]] = i;

	int n = poleRootLevels.size();
	int mid = n / 2;

	for (int o = 0; o < n; o++) {
		int level = (o % 2 == 0) ? (mid + o / 2) : (mid - 1 - o / 2);
		if (level < 0 || level >= n) continue;

		for (int pole = poleRootLevels[level].size() - 1; pole >= 0; pole--) {
			if (level == 0 && poleRootLevels[level][pole].first.x > 0.5 - PLATFORM_WIDTH * 0.36f) continue;
			if (level == poleRootLevels.size() - 1 && poleRootLevels[level][pole].first.x < 0.5 + PLATFORM_WIDTH * 0.36f) continue;

			int root1 = poleRootLevels[level][pole].second;
			int gIdx = globalIndexOfTrack[root1];
			float depth = 1.0f - std::abs(float(gIdx - globalMid) / float(globalMid));
			float floorDepth = 1 - std::abs(float(level - mid) / float(mid));
			if (depth < 0) depth = 0;

			Coords poleRoot = poleRootLevels[level][pole].first;
			Coords poleFloor(poleRoot.x, FLOOR_Y + floorDepth * (FLOOR_Y_MAX - FLOOR_Y));

			if (pole + 1 < poleRootLevels[level].size()) {
				Coords poleRoot2 = poleRootLevels[level][pole + 1].first;
				int root2 = poleRootLevels[level][pole + 1].second;
				if (std::abs(poleRoot2.x - poleRoot.x) < POLE_DISTANCE * 3.f) {
					float y = poleFloor.y + POLE_DISTANCE;
					while (y < poleRoot.y || y < poleRoot2.y) {
						if (y < poleRoot.y && y < poleRoot2.y)
							drawLine({ {poleRoot.x, y}, {poleRoot2.x, y} }, dimmColor(poleColor, DIMM_FACTOR, depth), POLE_WIDTH * SCAFFOLD_POLE_PERCENT, 0.1f, -90.f, POLE_WIDTH * 0.45);
						else {
							float refX = (y < poleRoot.y) ? poleRoot.x : poleRoot2.x;
							float nearestX = findClosestTrackX(y, root1, root2, tracks, refX);
							drawLine({ {refX, y}, {nearestX, y} }, dimmColor(poleColor, DIMM_FACTOR, depth), POLE_WIDTH * SCAFFOLD_POLE_PERCENT, 0.1f, -90.f, POLE_WIDTH * 0.45);
						}
						y += POLE_DISTANCE;
					}
				}
			}

			if (poleRoot.y > poleFloor.y)
				drawLine({ poleRoot, poleFloor }, dimmColor(poleColor, DIMM_FACTOR, depth), POLE_WIDTH * SCAFFOLD_POLE_PERCENT, 0.1f, 0.f, POLE_WIDTH * 0.45);
		}
	}
}

Simulation::Simulation(WindowManager& window, std::vector<Coords>& tracks, std::vector<PointStats>& points) :
	lineEngine(window), window(window), tracks(tracks), train(window, *this, tracks, points, BOTTOM_RATIO, MARGIN, 4, CHARACTERS),
	background(window, { "assets/background.png" }),
	platform(window, { "assets/platform.png" }) {
	std::random_device rd;
	std::default_random_engine eng(rd());
	std::uniform_real_distribution<float> dist(0, 0.5);
	for (int i = 0; i < MAX_LEVELS; i++)
		randomLevelOffsets.push_back(POLE_DISTANCE * dist(eng));
}

void Simulation::draw() {
	background.draw(Bounds(0.5f, 0.f, 1.f), { true });
	if (!tracks.empty()) {
		drawScaffolding();
		platform.draw(Bounds(0.5f, BOTTOM_RATIO, PLATFORM_WIDTH), { true });
		drawLine(tracks, Color(171, 87, 65), 5.2f, 0.2f, -80.f, 5.2f * 0.32f);
		train.draw();
	} else {
		platform.draw(Bounds(0.5f, BOTTOM_RATIO, PLATFORM_WIDTH), { true });
	}
}
