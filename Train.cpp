#include "Train.h"
#include "WindowManager.h"
#include "DataClasses.h"
#include "Simulation.h"
#include "GLFW/glfw3.h"
#include "Image.h"
#include <numbers>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

namespace {
	constexpr float CAR_WIDTH = 0.08f, CAR_PIVOT_OFFSET = 0.07f, CAR_DISTANCE = 0.078f;
	constexpr float CONNECTION_HEIGHT = 0.016f, CONNECTION_WIDTH = 3.8f, CONNECTION_RATIO = 0.6f;
	constexpr float BELT_HEIGHT_OFFSET = 0.4f, BELT_WIDTH = 0.009f, BELT_HEIGHT = 0.011f;
	constexpr float TRAIN_START_OFFSET = 0.045f, TRAIN_TRACKS_OFFSET = 0.006f;
	constexpr float PASSENGER_OFFSET = 0.15f;
	constexpr float APPEARANCE_SPEED = 0.01f;

	Color CONNECTION_COLOR(115, 118, 120);
}

Train::Train(WindowManager& window, Simulation& simulation, std::vector<Coords>& tracks, std::vector<PointStats>& points,
	float bottomRatio, float margin, int noCars, const std::vector<std::string>& characters) :
	lineEngine(window), window(window), simulation(simulation), tracks(tracks), points(points),
	BOTTOM_RATIO(bottomRatio), MARGIN(margin), noCars(noCars), characters(characters.size()) {
	std::vector<std::string> imagePaths = { "assets/train/seats.png" };
	for (std::string ext : {".png", "_sick.png"})
		for (int i = 0; i < characters.size(); i++)
			imagePaths.push_back("assets/characters/" + characters[i] + "/" + (!(i % 2) ? "right" : "left") + ext);
	imagePaths.insert(imagePaths.end(), { "assets/train/belt_left.png",
										  "assets/train/belt_right.png",
										  "assets/train/cart_front.png",
										  "assets/train/cart.png" });
	carImage = new Image(window, imagePaths);
	window.addKeyboardListener(this);
	window.addMouseListener(this);
}

void Train::keyboardCallback(GLFWwindow& window, int key, int scancode, int action, int mods) {
	if (action != GLFW_PRESS) return;

	if (key == GLFW_KEY_SPACE) {
		for (int i = 0; i < characters.size(); i++) {
			if (!characters[i].character) {
				characters[i].character = APPEARANCE_SPEED;
				break;
			}
		}
	} else if (key == GLFW_KEY_ENTER) {
		int count = 0;
		for (int i = 0; i < characters.size(); i++) {
			if (characters[i].character) {
				if (!characters[i].belt) return;
				count++;
			}
		}
		if (count)
			mode = 1;
	}
}

int Train::seatHit(float x, float y) {
	Coords mp = toRelative(Coords(x, y));
	PointStats start = closestPoint(-TRAIN_START_OFFSET);

	for (int j = 0; j < noCars; j++) {
		float px = start.crds.x - CAR_WIDTH * PASSENGER_OFFSET + j * CAR_WIDTH * 0.97f;
		float py = start.crds.y + BOTTOM_RATIO + CAR_WIDTH * BELT_HEIGHT_OFFSET;

		if (mp.x > px - BELT_WIDTH && mp.x < px + BELT_WIDTH &&
			mp.y > py - BELT_HEIGHT && mp.y < py + BELT_HEIGHT &&
			characters[j * 2].character) return j * 2;

		px += CAR_WIDTH * PASSENGER_OFFSET * 2.f;

		if (mp.x > px - BELT_WIDTH && mp.x < px + BELT_WIDTH &&
			mp.y > py - BELT_HEIGHT && mp.y < py + BELT_HEIGHT &&
			characters[j * 2 + 1].character) return j * 2 + 1;
	}
	return -1;
}

void Train::mouseCallback(double x, double y, int button, int action, int mods) {
	if (button != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_PRESS) return;

	int hit = seatHit(x, y);
	if (hit > -1 && !characters[hit].belt)
		characters[hit].belt = APPEARANCE_SPEED;
}

inline static Coords edgePoint(Bounds b, float R, float angle, bool right, float pivotHeight) {
	float W = b.width * CONNECTION_RATIO;
	float x = (right ? 0.5f : -0.5f) * W;
	float py = R * pivotHeight;

	float dy = R - py;
	float a = angle * (std::numbers::pi_v<float> / 180.f);
	float c = std::cos(a), s = std::sin(a);

	float X = x * c - dy * s;
	float Y = x * s + dy * c + py;

	return { b.x + X, b.y + Y };
}

Bounds Train::toContent(Bounds bounds) const {
	Bounds b = simulation.trackBounds();
	bounds.x = bounds.x * b.width / window.getWidth() + b.x / window.getWidth();
	bounds.y = bounds.y * b.width / window.getWidth() + TRAIN_TRACKS_OFFSET;
	return bounds;
}

Coords Train::toScreen(Coords point) const {
	Bounds b = simulation.trackBounds();
	float unit = b.width;
	float x = b.x + point.x * b.width;
	float y = window.getHeight() - (point.y * unit);
	return { x, y };
}

Coords Train::toRelative(Coords point) const {
	Bounds b = simulation.trackBounds();
	point.x = (point.x - MARGIN) / b.width;
	point.y = (window.getHeight() - point.y) / b.width;
	return point;
}

PointStats Train::closestPoint(float dist) {
	static PointStats dummy;
	if (points.empty()) return dummy;

	float total = points.back().distance;
	if (total <= 0.f) return points[0];

	float mod = std::fmod(dist, total);
	if (mod < 0) mod += total;

	size_t hi = 1;
	while (hi < points.size() && points[hi].distance < mod) hi++;
	size_t lo = hi - 1;

	const PointStats& A = points[lo];
	const PointStats& B = points[hi < points.size() ? hi : lo];

	if (A.distance == B.distance) return A;

	float t = (mod - A.distance) / (B.distance - A.distance);

	PointStats r;
	r.firstHalf = (t < 0.5f ? A.firstHalf : B.firstHalf);
	r.distance = mod;
	r.angle = A.angle + t * (B.angle - A.angle);
	r.crds.x = A.crds.x + t * (B.crds.x - A.crds.x);
	r.crds.y = A.crds.y + t * (B.crds.y - A.crds.y);

	return r;
}

std::vector<float> Train::getLayers(int carIdx) {
	std::vector<float> layers(5 + characters.size() * 2);
	int chrIdx1 = carIdx * 2;
	int chrIdx2 = carIdx * 2 + 1;
	layers[layers.size() - 1] = float(carIdx > 0);
	layers[layers.size() - 2] = float(!carIdx);
	layers[layers.size() - 3] = characters[chrIdx1].belt;
	layers[layers.size() - 4] = characters[chrIdx2].belt;
	layers[1 + chrIdx1] = characters[chrIdx1].character;
	layers[1 + chrIdx2] = characters[chrIdx2].character;
	layers[1 + characters.size() + chrIdx1] = characters[chrIdx1].sick;
	layers[1 + characters.size() + chrIdx2] = characters[chrIdx2].sick;
	layers[0] = 1;
	return layers;
}

void Train::draw() {
	static float animCounter = -TRAIN_START_OFFSET;

	if (!tracks.empty()) {
		//if (animCounter < points.back().distance - TRAIN_START_OFFSET)
		if (mode)
			animCounter += 0.0002f;

		std::vector<Bounds> cars;
		bool secondHalf = false;
		for (int i = 0; i < noCars; i++) {
			PointStats point = closestPoint(animCounter - i * CAR_DISTANCE);
			cars.push_back(Bounds(point.crds.x, BOTTOM_RATIO + point.crds.y, CAR_WIDTH, 0, point.angle, point.firstHalf));
			secondHalf = i == 3 && !point.firstHalf;
		}


		for (int i = 1; i < cars.size(); i++) {
			Coords left = edgePoint(cars[i - 1], CONNECTION_HEIGHT, cars[i - 1].angle, cars[i - 1].flip, CAR_PIVOT_OFFSET);
			Coords right = edgePoint(cars[i], CONNECTION_HEIGHT, cars[i].angle, !cars[i].flip, CAR_PIVOT_OFFSET);
			simulation.drawLine({ left, right }, CONNECTION_COLOR, CONNECTION_WIDTH, 0.2f, -90, CONNECTION_WIDTH * 0.45, false);
		}


		for (auto& chr : characters) {
			if (chr.character) chr.character = std::min(1.f, chr.character + APPEARANCE_SPEED);
			if (chr.sick) chr.sick = std::min(1.f, chr.sick + APPEARANCE_SPEED);
			if (chr.belt) chr.belt = std::min(1.f, chr.belt + APPEARANCE_SPEED);
		}

		if (secondHalf) for (int i = cars.size() - 1; i >= 0; i--) {
			carImage->draw(toContent(cars[i]), getLayers(i), cars[i].flip, cars[i].angle, CAR_PIVOT_OFFSET);
		} else for (int i = 0; i < cars.size(); i++) {
			carImage->draw(toContent(cars[i]), getLayers(i), cars[i].flip, cars[i].angle, CAR_PIVOT_OFFSET);
		}
	}

}
