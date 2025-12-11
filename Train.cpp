#include "Train.h"
#include "WindowManager.h"
#include "DataClasses.h"
#include "Simulation.h"
#include "GLFW/glfw3.h"
#include "Character.h"
#include <algorithm>
#include "Image.h"
#include <numbers>
#include <chrono>
#include <random>
#include <vector>
#include <string>
#include <cmath>

namespace {
	constexpr float CAR_WIDTH = 0.08f, CAR_PIVOT_OFFSET = 0.07f, CAR_DISTANCE = 0.078f;
	constexpr float CONNECTION_HEIGHT = 0.016f, CONNECTION_WIDTH = 3.8f, CONNECTION_RATIO = 0.6f;
	constexpr float BELT_HEIGHT_OFFSET = 0.4f, BELT_WIDTH = 0.009f, BELT_HEIGHT = 0.011f;
	constexpr float TRAIN_START_OFFSET = 0.045f, TRAIN_TRACKS_OFFSET = 0.006f;
	constexpr float APPEARANCE_SPEED = 0.013f;
	constexpr float PASSENGER_OFFSET = 0.15f;

	constexpr float SLOWDOWN_DISTANCE = 0.08f, FINISH_SLOWDOWN_DISTANCE = 0.3f, FINISH_SLOWDOWN_DISTANCE_SICK = 0.15f, FINISHED_DISTANCE = 0.0002f;
	constexpr float TRAIN_MIN_SPEED = 0.11f, TRAIN_MAX_SPEED = 0.58f, TRAIN_MAX_SPEED_SICK = 0.1f;
	constexpr float TRAIN_FLAT_ACCEL = 0.15f, TRAIN_SLOPE_FACTOR = 0.75f;

	Color CONNECTION_COLOR(115, 118, 120);
}

Train::Train(WindowManager& window, Simulation& simulation, std::vector<Coords>& tracks, std::vector<PointStats>& points,
	float bottomRatio, float margin, int noCars, const std::vector<std::string>& characters) :
	lineEngine(window), window(window), simulation(simulation), tracks(tracks), points(points),
	BOTTOM_RATIO(bottomRatio), MARGIN(margin), noCars(noCars), characters(characters.size()) {
	std::vector<std::string> imagePaths = { "assets/train/seats.png" };
	for (std::string ext : {".png", "_sick.png"}) {
		for (int i = 0; i < characters.size(); i++) {
			imagePaths.push_back("assets/characters/" + characters[i] + "/" + (!(i % 2) ? "right" : "left") + ext);
			this->characters[i].walkImage = new Image(window, { "assets/characters/" + characters[i] + "/walk.png" });
			this->characters[i].idleImage = new Image(window, { "assets/characters/" + characters[i] + "/idle.png" });
			this->characters[i].idx = i;
		}
	}
	imagePaths.insert(imagePaths.end(), { "assets/train/belt_left.png",
										  "assets/train/belt_right.png",
										  "assets/train/cart_front.png",
										  "assets/train/cart.png" });
	carImage = new Image(window, imagePaths);
	currentDistance = -TRAIN_START_OFFSET;
	stopDistance = currentDistance;
	window.addKeyboardListener(this);
	window.addMouseListener(this);
}

void Train::shuffle() {
	std::vector<Character> even, odd;
	even.reserve(characters.size() / 2);
	odd.reserve(characters.size() / 2);

	for (size_t i = 0; i < characters.size(); i++) {
		if (i % 2) odd.push_back(characters[i]);
		else       even.push_back(characters[i]);
	}

	std::random_device rd;
	std::mt19937 gen(rd());
	std::shuffle(even.begin(), even.end(), gen);
	std::shuffle(odd.begin(), odd.end(), gen);

	size_t ei = 0, oi = 0;
	for (size_t i = 0; i < characters.size(); i++) {
		if (i % 2) characters[i] = odd[oi++];
		else       characters[i] = even[ei++];
	}
}

void Train::reset() {
	currentDistance = -TRAIN_START_OFFSET;
	currentSpeed = 0;
	mode = 0;

	shuffle();

	for (auto& chr : characters) {
		chr.position = -1.f;
		chr.character = 0;
		chr.walker = 0;
		chr.mode = 0;
		chr.idle = 0;
		chr.sick = 0;
		chr.belt = 0;
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

float Train::seatFor(int passenger) {
	PointStats start = closestPoint(-TRAIN_START_OFFSET);
	float px = start.crds.x - CAR_WIDTH * PASSENGER_OFFSET + float(int(passenger / 2)) * CAR_WIDTH * 0.97f;
	return passenger % 2 ? px + CAR_WIDTH * PASSENGER_OFFSET * 2.f : px;
}

void Train::mouseCallback(double x, double y, int button, int action, int mods) {
	if (button != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_PRESS) return;

	int hit = seatHit(x, y);
	if (hit > -1 && characters[hit].belt <= 0.f && mode == 0)
		characters[hit].belt = APPEARANCE_SPEED;

	if (hit > -1 && characters[hit].character > 0.f && mode == 4) {
		characters[hit].walk(false, seatFor(hit));
		characters[hit].character = -1.f;
		if (characters[hit].sick > 0.f)
			characters[hit].sick = -1.f;
	}
}

void Train::keyboardCallback(GLFWwindow& window, int key, int scancode, int action, int mods) {
	if (action != GLFW_PRESS) return;

	if (key == GLFW_KEY_SPACE && mode == 0) {
		for (int i = 0; i < characters.size(); i++) {
			if (characters[i].character <= 0.f && !characters[i].mode) {
				characters[i].walk(true, seatFor(i));
				break;
			}
		}
	} else if (key == GLFW_KEY_ENTER && mode == 0) {
		int count = 0;
		for (int i = 0; i < characters.size(); i++) {
			if (characters[i].walker > 0.f) return;
			if (characters[i].character > 0.f) {
				if (characters[i].belt <= 0.f) return;
				count++;
			}
		}
		if (count)
			mode = 1;
	} else if (key == GLFW_KEY_R) {
		reset();
	} else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_8 && mode == 1) {
		int number = key - GLFW_KEY_1;
		if (number < characters.size() && characters[number].character > 0) {
			stopDistance = currentDistance + SLOWDOWN_DISTANCE;
			characters[number].sick = APPEARANCE_SPEED;
			mode = 2;
		}
	}
}

void Train::updateDistance() {
	float dt;
	int tryAgain = 2;
	while (tryAgain > 0) {
		auto now = std::chrono::steady_clock::now();
		dt = std::chrono::duration<float>(now - lastUpdateTime).count();
		lastUpdateTime = now;
		if (dt < 0.5) tryAgain--;
		tryAgain--;
	}

	if (sleep >= 0.f) {
		sleep -= dt;
		return;
	}

	if (mode == 1) {
		std::vector<float> currentSpeeds(noCars, currentSpeed);

		for (int i = 0; i < noCars; i++) {
			PointStats p = closestPoint(currentDistance - i * CAR_DISTANCE);

			float ang = std::fmod(p.angle + (p.firstHalf ? 180.0f : 0.0f), 360.0f);
			if (ang > 180.0f) ang -= 360.0f;
			float slope = -std::sin(ang * (std::numbers::pi_v<float> / 180.0f));

			float accel = TRAIN_FLAT_ACCEL + slope * TRAIN_SLOPE_FACTOR;
			if (currentSpeeds[i] < TRAIN_MIN_SPEED) accel = TRAIN_SLOPE_FACTOR;

			currentSpeeds[i] += accel * dt;
			currentSpeeds[i] = std::clamp(currentSpeeds[i], 0.0f, TRAIN_MAX_SPEED);

			float endDist = points.back().distance - TRAIN_START_OFFSET;
			float remaining = endDist - currentDistance;

			if (remaining <= FINISH_SLOWDOWN_DISTANCE) {
				float t = std::clamp(remaining / FINISH_SLOWDOWN_DISTANCE, 0.0f, 1.0f);
				currentSpeeds[i] = preStopSpeed * std::pow(t, 0.82f);

				if (remaining <= FINISHED_DISTANCE) {
					currentSpeeds[i] = 0.0f;
					currentDistance = endDist;
					for (auto& chr : characters)
						if (chr.belt > 0) chr.belt = -1.f;
					mode = 4;
					return;
				}
			} else {
				preStopSpeed = currentSpeed;
			}
		}

		if (!currentSpeeds.empty()) {
			float weightedSum = 0.0f;
			float totalWeight = 0.0f;
			int count = currentSpeeds.size();

			for (int i = 0; i < count; i++) {
				float weight = float(count - i);
				weightedSum += currentSpeeds[i] * weight;
				totalWeight += weight;
			}

			currentSpeed = weightedSum / totalWeight;
			currentDistance += currentSpeed * dt;
		}
	} else if (mode == 2) {
		float remaining = stopDistance - currentDistance;
		float t = std::clamp(remaining / SLOWDOWN_DISTANCE, 0.0f, 1.0f);
		currentSpeed = preStopSpeed * std::pow(t, 0.82f);
		if (remaining <= FINISHED_DISTANCE) {
			currentDistance = stopDistance;
			currentSpeed = 0.0f;
			sleep = 10;
			mode = 3;
		}
		currentDistance += currentSpeed * dt;
	} else if (mode == 3) {
		float accel = TRAIN_FLAT_ACCEL;
		currentSpeed += accel * dt;
		currentSpeed = std::clamp(currentSpeed, 0.0f, TRAIN_MAX_SPEED_SICK);

		float endDist = points.back().distance - TRAIN_START_OFFSET;
		float remaining = endDist - currentDistance;

		if (remaining <= FINISH_SLOWDOWN_DISTANCE_SICK) {
			float t = std::clamp(remaining / FINISH_SLOWDOWN_DISTANCE_SICK, 0.0f, 1.0f);
			currentSpeed = preStopSpeed * std::pow(t, 0.7f);

			if (remaining <= FINISHED_DISTANCE) {
				currentSpeed = 0.0f;
				currentDistance = endDist;
				for (auto& chr : characters)
					if (chr.belt > 0) chr.belt = -1.f;
				mode = 4;
				return;
			}
		} else {
			preStopSpeed = currentSpeed;
		}

		currentDistance += currentSpeed * dt;
	}
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
	int imgIdx1 = characters[chrIdx1].idx;
	int imgIdx2 = characters[chrIdx2].idx;
	layers[layers.size() - 1] = float(carIdx > 0);
	layers[layers.size() - 2] = float(!carIdx);
	layers[layers.size() - 3] = std::fabs(characters[chrIdx1].belt);
	layers[layers.size() - 4] = std::fabs(characters[chrIdx2].belt);
	layers[1 + imgIdx1] = std::fabs(characters[chrIdx1].character);
	layers[1 + imgIdx2] = std::fabs(characters[chrIdx2].character);
	layers[1 + characters.size() + imgIdx1] = std::fabs(characters[chrIdx1].sick);
	layers[1 + characters.size() + imgIdx2] = std::fabs(characters[chrIdx2].sick);
	layers[0] = 1;
	return layers;
}

void Train::draw() {
	if (tracks.size() != pointsLength) {
		pointsLength = tracks.size();
		reset();
	}

	if (!tracks.empty()) {
		updateDistance();

		std::vector<Bounds> cars;
		bool secondHalf = false;
		for (int i = 0; i < noCars; i++) {
			PointStats point = closestPoint(currentDistance - i * CAR_DISTANCE);
			cars.push_back(Bounds(point.crds.x, BOTTOM_RATIO + point.crds.y, CAR_WIDTH, 0, point.angle, point.firstHalf));
			secondHalf = i == 3 && !point.firstHalf;
		}

		for (int i = 1; i < cars.size(); i++) {
			Coords left = edgePoint(cars[i - 1], CONNECTION_HEIGHT, cars[i - 1].angle, cars[i - 1].flip, CAR_PIVOT_OFFSET);
			Coords right = edgePoint(cars[i], CONNECTION_HEIGHT, cars[i].angle, !cars[i].flip, CAR_PIVOT_OFFSET);
			simulation.drawLine({ left, right }, CONNECTION_COLOR, CONNECTION_WIDTH, 0.2f, -90, CONNECTION_WIDTH * 0.45f, false);
		}

		for (auto& chr : characters)
			chr.updateLayers();

		if (mode == 4) {
			bool allGone = true;
			for (auto& chr : characters)
				allGone &= chr.character == 0.f && chr.walker == 0.f;
			if (allGone) reset();
		}

		if (secondHalf) for (int i = cars.size() - 1; i >= 0; i--)
			carImage->draw(toContent(cars[i]), getLayers(i), cars[i].flip, cars[i].angle, CAR_PIVOT_OFFSET);
		else for (int i = 0; i < cars.size(); i++)
			carImage->draw(toContent(cars[i]), getLayers(i), cars[i].flip, cars[i].angle, CAR_PIVOT_OFFSET);

		for (auto& chr : characters)
			chr.draw();
	}
}
