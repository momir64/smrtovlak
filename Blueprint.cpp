#pragma once 
#include "Blueprint.h"
#include "WindowManager.h" 
#include "DataClasses.h" 
#include "PulseEngine.h" 
#include "GLFW/glfw3.h" 
#include "LineEngine.h" 
#include <algorithm>
#include "Button.h" 
#include <numbers>
#include <fstream>
#include <utility>
#include <vector>
#include <string>
#include <cmath>

namespace {
	constexpr float MARGIN = 50.f, TOP = 150.f;

	constexpr float PLATFORM_WIDTH_RATIO = 0.28f, PLATFORM_HEIGHT_RATIO = 0.04f;
	constexpr float PLATFORM_BASE_STEP = 3.f, PLATFORM_STEP_RATIO = 0.003f;
	constexpr float PLATFORM_DIAG_RATIO = 0.2f, PLATFORM_MIN_DIAG = 10.f;
	constexpr float PLATFORM_BORDER_MAX = 7.f, PLATFORM_LINE_MAX = 4.f;
	constexpr float PLATFORM_ANGLE_DEG = 40.f;

	constexpr float GRID_MIN_SPACING = 75.f, GRID_SPACING_RATIO = 0.08f;
	constexpr int GRID_SUBDIVISIONS = 5;

	constexpr float PULSE_SIZE_FACTOR = 0.0012f, PULSE_MIN_SIZE = 0.03f;
	constexpr float PULSE_SPEED_BASE = 320.f, PULSE_SPEED_MIN = 360.f;
	constexpr float PULSE_ASPECT = 0.32f, PULSE_HOVER_SCALE = 1.75f;
	constexpr float PULSE_OFFSET = 0.28f, PULSE_STRENGTH = 0.46f;

	inline float deg2rad(float d) { return d * (std::numbers::pi_v<float> / 180.f); }
}

Blueprint::Blueprint(WindowManager& win, std::vector<Coords>& tracks, const std::string& trackPath) :
	window(win), drawing(tracks), pulse(win), line(win), trackPath(trackPath),
	trash(win, Bounds(166, 20, 100, 0), Color(183, 198, 215), Color(255, 255, 255),
		0.15f, 16, { "assets/icons/trash.png", "assets/icons/trash.png" }) {
	window.addKeyboardListener(this);
	trash.setListener(this);
}

Bounds Blueprint::contentBounds() const {
	return Bounds(MARGIN, TOP,
		window.getWidth() - MARGIN * 2.f,
		window.getHeight() - MARGIN - TOP);
}

Bounds Blueprint::platformBounds() const {
	Bounds c = contentBounds();
	float pw = c.width * PLATFORM_WIDTH_RATIO;
	float ph = c.width * PLATFORM_HEIGHT_RATIO;
	return Bounds(c.x + c.width * 0.5f - pw * 0.5f, c.y + c.height - ph, pw, ph);
}

bool Blueprint::insideContent(float mx, float my) const {
	Bounds b = contentBounds();
	return mx >= b.x && mx <= b.x + b.width && my >= MARGIN && my <= window.getHeight() - TOP;
}

Coords Blueprint::toLocal(float mx, float my) const {
	Bounds b = contentBounds();
	return Coords((mx - b.x) / b.width, (my - MARGIN) / b.width);
}

Coords Blueprint::clipToContent(float mx, float my) const {
	Bounds b = contentBounds();
	float xmax = b.x + b.width, ymax = window.getHeight() - TOP;
	if (mx < b.x) mx = b.x; else if (mx > xmax) mx = xmax;
	if (my < MARGIN) my = MARGIN; else if (my > ymax) my = ymax;
	return toLocal(mx, my);
}

int Blueprint::detectPulse(float mx, float my) const {
	Bounds p = platformBounds();
	float w = window.getWidth(), h = window.getHeight();

	float cx1 = p.x / w, cy = (h - p.y) / h;
	float cx2 = (p.x + p.width) / w;

	float nx = mx / w, ny = my / h;

	float size = std::max(PULSE_SIZE_FACTOR * std::sqrt(p.height), PULSE_MIN_SIZE) * 0.75f;
	float aspect = w / h;

	float dx1 = nx - cx1, dy1 = (ny - cy) / aspect;
	float dx2 = nx - cx2, dy2 = (ny - cy) / aspect;
	float d1 = std::sqrt(dx1 * dx1 + dy1 * dy1);
	float d2 = std::sqrt(dx2 * dx2 + dy2 * dy2);

	if (d1 <= size) return 1;
	if (d2 <= size) return 2;
	return 0;
}

void Blueprint::keyboardCallback(GLFWwindow&, int key, int, int action, int) {
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		drawingActive = true;
		drawing.clear();
		saveTrack();
	}
}

void Blueprint::buttonChanged(Button&, int) {
	drawingActive = true;
	drawing.clear();
	saveTrack();
}

void Blueprint::mouseCallback(double x, double y, int button, int action, int) {
	float mx = float(x), my = float(window.getHeight() - y);

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && drawingActive && drawing.empty() && (hoveringPulse = detectPulse(mx, my)) && insideContent(mx, my)) {
		drawing.push_back(toLocal(mx, my));
		startingPulse = hoveringPulse;
		return;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE && !drawing.empty() && drawingActive) {
		bool finishedOpposite = (hoveringPulse == 3 - startingPulse);
		drawingActive = !finishedOpposite;
		if (finishedOpposite) finalizeAndCloseLine(); else drawing.clear();
		startingPulse = 0;
		saveTrack();
	}
}

void Blueprint::cursorCallback(double x, double y) {
	float mx = float(x), my = float(window.getHeight() - y);
	hoveringPulse = detectPulse(mx, my);
	if (!drawingActive || drawing.empty()) return;
	drawing.push_back(clipToContent(mx, my));
}

void Blueprint::drawBorder() {
	Bounds b = contentBounds();
	line.draw({ {b.x,b.y},{b.x + b.width,b.y},
				{b.x + b.width,b.y + b.height},
				{b.x,b.y + b.height},{b.x,b.y} }, Color(255, 255, 255), 6);
}

void Blueprint::drawGrid() {
	Bounds b = contentBounds();
	float spacing = std::max(window.getWidth() * GRID_SPACING_RATIO, GRID_MIN_SPACING);
	int rows = int(b.height / spacing), cols = int(b.width / spacing);
	float rSpace = b.height / rows, cSpace = b.width / cols;
	Color major(131, 143, 155), minor(113, 129, 145);

	for (int r = 1; r <= rows; r++) {
		for (int s = 1; s < GRID_SUBDIVISIONS; s++) {
			float y = b.y + (r - 1) * rSpace + s * (rSpace / GRID_SUBDIVISIONS);
			line.draw({ {b.x,y},{b.x + b.width,y} }, minor, 2);
		}
	}

	for (int c = 1; c <= cols; c++) {
		for (int s = 1; s < GRID_SUBDIVISIONS; s++) {
			float x = b.x + (c - 1) * cSpace + s * (cSpace / GRID_SUBDIVISIONS);
			line.draw({ {x,b.y},{x,b.y + b.height} }, minor, 2);
		}
	}

	for (int r = 0; r <= rows; r++) {
		float y = b.y + r * rSpace;
		line.draw({ {b.x,y},{b.x + b.width,y} }, major, 4);
	}

	for (int c = 0; c <= cols; c++) {
		float x = b.x + c * cSpace;
		line.draw({ {x,b.y},{x,b.y + b.height} }, major, 4);
	}
}

void Blueprint::drawPlatform() {
	Bounds p = platformBounds();
	Color col(195, 200, 205);
	float w = window.getWidth();

	line.draw({ {p.x,p.y},{p.x + p.width,p.y},
				{p.x + p.width,p.y + p.height},
				{p.x,p.y + p.height},{p.x,p.y} },
		col, std::min(PLATFORM_BORDER_MAX, w * 0.006f));

	float diag = std::max(p.height * PLATFORM_DIAG_RATIO, PLATFORM_MIN_DIAG);
	float step = std::min(PLATFORM_BASE_STEP, w * PLATFORM_STEP_RATIO) * 3.f;

	float ang = deg2rad(PLATFORM_ANGLE_DEG);
	float vx = -std::cos(ang), vy = std::sin(ang);

	float xmin = p.x, xmax = p.x + p.width;

	for (float x = p.x; x < p.x + p.width + diag; x += step) {
		float x0 = x, y0 = p.y;
		float x1 = x + vx * diag, y1 = p.y + vy * diag;
		if ((x0 < xmin && x1 < xmin) || (x0 > xmax && x1 > xmax)) continue;

		float dx = x1 - x0, dy = y1 - y0;
		float t0 = (x0 < xmin) ? (xmin - x0) / dx : (x0 > xmax) ? (xmax - x0) / dx : 0.f;
		float t1 = (x1 < xmin) ? (xmin - x0) / dx : (x1 > xmax) ? (xmax - x0) / dx : 1.f;
		if (t0 > t1) std::swap(t0, t1);

		line.draw({ {x0 + dx * t0,y0 + dy * t0},{x0 + dx * t1,y1 + dy * t1} }, col, std::min(PLATFORM_LINE_MAX, w * 0.004f));
	}
}

void Blueprint::drawPulses() {
	Bounds p = platformBounds();
	Color white(255, 255, 255);

	float w = window.getWidth(), h = window.getHeight();
	float x1 = p.x / w, x2 = (p.x + p.width) / w, y = p.y / h;

	float size = std::max(PULSE_SIZE_FACTOR * std::sqrt(p.height), PULSE_MIN_SIZE);
	float speed = std::max(PULSE_SPEED_BASE / std::pow(p.height, 0.01f), PULSE_SPEED_MIN);

	if (startingPulse != 1 && drawingActive)
		pulse.draw(x1, y, size, white, PULSE_STRENGTH, PULSE_ASPECT, speed, hoveringPulse == 1 ? PULSE_HOVER_SCALE : 1.f);

	if (startingPulse != 2 && drawingActive)
		pulse.draw(x2, y, size, white, PULSE_STRENGTH, PULSE_ASPECT, speed, hoveringPulse == 2 ? PULSE_HOVER_SCALE : 1.f);
}

void Blueprint::drawDrawing() {
	if (drawing.size() < 2) return;
	Bounds b = contentBounds();
	float unit = b.width;

	std::vector<Coords> pts;
	pts.reserve(drawing.size());
	for (auto& p : drawing) {
		float x = b.x + p.x * b.width;
		float y = std::max(TOP, window.getHeight() - (MARGIN + p.y * unit));
		pts.emplace_back(x, y);
	}

	line.draw(pts, Color(255, 255, 255), 6);
}

static inline void addHermite(std::vector<Coords>& out, const Coords& p0, const Coords& t0, const Coords& p1, const Coords& t1, int steps) {
	for (int i = 1; i <= steps; i++) {
		float t = float(i) / steps, tt = t * t, ttt = tt * t;
		float h0 = 2.f * ttt - 3.f * tt + 1.f;
		float h1 = ttt - 2.f * tt + t;
		float h2 = -2.f * ttt + 3.f * tt;
		float h3 = ttt - tt;
		out.emplace_back(
			h0 * p0.x + h1 * t0.x + h2 * p1.x + h3 * t1.x,
			h0 * p0.y + h1 * t0.y + h2 * p1.y + h3 * t1.y
		);
	}
}

static inline Coords avgDir(const std::vector<Coords>& v, int start, int count) {
	float sx = 0.f, sy = 0.f; int used = 0;
	for (int i = 0; i < count; i++) {
		int a = start + i, b = a + 1;
		if (b >= (int)v.size()) break;
		sx += v[b].x - v[a].x;
		sy += v[b].y - v[a].y;
		used++;
	}
	if (used == 0) return { 1.f, 0.f };
	float L = std::sqrt(sx * sx + sy * sy);
	return (L < 1e-6f) ? Coords{ 1.f, 0.f } : Coords{ sx / L, sy / L };
}

void Blueprint::saveTrack() {
	std::ofstream file(trackPath);
	if (!file.is_open()) return;
	for (const auto& c : drawing)
		file << c.x << " " << c.y << "\n";
}

void Blueprint::loadTrack() {
	std::ifstream file(trackPath);
	if (!file.is_open()) return;
	drawing.clear();
	float x, y;
	while (file >> x >> y)
		drawing.emplace_back(x, y);
	if (!drawing.empty())
		drawingActive = false;
}

void Blueprint::finalizeAndCloseLine() {
	if (drawing.size() < 2) return;

	if (drawing.front().x > drawing.back().x)
		std::reverse(drawing.begin(), drawing.end());

	Bounds p = platformBounds();
	float wh = window.getHeight();

	Coords l = toLocal(p.x, wh - p.y);
	Coords r = toLocal(p.x + p.width, wh - p.y);
	float trim = l.y * 1.5f;

	size_t L = 0;
	while (L + 1 < drawing.size() && drawing[L].x > l.x - trim) L++;

	size_t R = drawing.size() - 1;
	while (R > L + 1 && drawing[R].x < r.x + trim) R--;

	drawing.erase(drawing.begin() + R + 1, drawing.end());
	drawing.erase(drawing.begin(), drawing.begin() + L);
	if (drawing.size() < 2) return;

	Coords A = drawing.front();
	Coords B = drawing.back();

	Coords dirA = avgDir(drawing, 0, 5);
	Coords dirB = avgDir(drawing, (int)drawing.size() - 6, 5);

	float curve = l.y * 2.4f;

	Coords t0_left = { -curve, 0.f };
	Coords t1_left = { dirA.x * curve, dirA.y * curve };
	Coords t0_right = { -curve, 0.f };
	Coords t1_right = { dirB.x * curve, dirB.y * curve };

	std::vector<Coords> out;
	out.reserve(drawing.size() + 48);

	out.push_back(l);
	addHermite(out, l, t0_left, A, t1_left, 24);

	for (auto& q : drawing)
		out.push_back(q);

	addHermite(out, B, t1_right, r, t0_right, 24);
	out.push_back(r);

	int steps = 24;
	for (int i = 1; i <= steps; i++) {
		float t = float(i) / steps;
		out.emplace_back(r.x + (l.x - r.x) * t, r.y + (l.y - r.y) * t);
	}

	drawing.swap(out);
}

void Blueprint::draw() {
	drawGrid();
	drawPlatform();
	drawBorder();
	drawPulses();
	drawDrawing();
	trash.draw();
}
