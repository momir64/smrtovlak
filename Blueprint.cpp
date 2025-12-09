#include "Blueprint.h"
#include "WindowManager.h"
#include "DataClasses.h"
#include "GLFW/glfw3.h"
#include <algorithm>
#include <iostream>
#include "Button.h"
#include <utility>
#include <numbers>
#include <vector>
#include <string>
#include <cmath>

Blueprint::Blueprint(WindowManager& window, std::vector<Coords>& tracks) :
	window(window), line(window), pulse(window), drawing(tracks), top(150), margin(50),
	trash(window, Bounds(166, 20, 100), Color(183, 198, 215), Color(1.0f, 1.0f, 1.0f),
		0.15f, 16, std::vector<std::string>{ "assets/icons/trash.png", "assets/icons/trash.png"}) {
	window.addKeyboardListener(this);
	trash.setListener(this);
}

void Blueprint::keyboardCallback(GLFWwindow& window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		currentlyDrawing = true;
		drawing.clear();
	}
}

void Blueprint::buttonChanged(Button& button, int selected) {
	currentlyDrawing = true;
	drawing.clear();
}

void Blueprint::drawBorder() {
	float height = window.getHeight() - margin - top;
	float width = window.getWidth() - margin * 2;
	Color borderColor(255, 255, 255);

	std::vector<Coords> border = {
		{margin, top}, {width + margin, top},
		{width + margin, height + top},
		{margin, height + top}, {margin, top}
	};

	line.draw(border, borderColor, 6);
}

void Blueprint::drawMesh() {
	float height = window.getHeight() - margin - top;
	float width = window.getWidth() - margin * 2, space = std::max(window.getWidth() * 0.08f, 75.f);
	int rows = int(height / space), cols = int(width / space);
	float cSpace = width / float(cols), rSpace = height / float(rows);
	//Color lineColor(200, 215, 230), subLineColor(149, 171, 193);
	Color lineColor(131, 143, 155), subLineColor(113, 129, 145);

	for (int r = 1; r <= rows; r++) {
		for (int s = 1; s < 5; s++) {
			float sy = top + (r - 1) * rSpace + s * (rSpace / 5.0f);
			std::vector<Coords> subLine = { {margin, sy}, {width + margin, sy} };
			line.draw(subLine, subLineColor, 2);
		}
	}

	for (int c = 1; c <= cols; c++) {
		for (int s = 1; s < 5; s++) {
			float sx = margin + (c - 1) * cSpace + s * (cSpace / 5.0f);
			std::vector<Coords> subLine = { {sx, top}, {sx, height + top} };
			line.draw(subLine, subLineColor, 2);
		}
	}

	for (int r = 0; r <= rows; r++) {
		float y = top + r * rSpace;
		std::vector<Coords> rowLine = { {margin, y}, {width + margin, y} };
		line.draw(rowLine, lineColor, 4);
	}

	for (int c = 0; c <= cols; c++) {
		float x = margin + c * cSpace;
		std::vector<Coords> colLine = { {x, top}, {x, height + top} };
		line.draw(colLine, lineColor, 4);
	}
}

void Blueprint::drawPlatform() {
	Color color(195, 200, 205);
	float ww = window.getWidth();

	float width = ww * 0.28f;
	float height = ww * 0.04f;

	float dy = std::max(height * 0.32f, 17.f);
	float step = std::min(3.f, ww * 0.003f) * 3.f;

	float winWidth = window.getWidth() - margin * 2;
	float winHeight = window.getHeight() - margin - top;

	float px = margin + winWidth * 0.5f - width * 0.5f;
	float py = top + winHeight - height;

	std::vector<Coords> r = {
		{px, py}, {px + width, py},
		{px + width, py + height},
		{px, py + height}, {px, py}
	};
	line.draw(r, color, std::min(7.f, ww * 0.006f));

	float ang = 46.f * (std::numbers::pi / 180.f);
	float vx = -cosf(ang);
	float vy = sinf(ang);

	for (float x = px; x < px + width + dy; x += step) {
		float x0 = x, y0 = py;
		float x1 = x + vx * dy, y1 = py + vy * dy;

		float xmin = px, xmax = px + width;
		if ((x0 < xmin && x1 < xmin) || (x0 > xmax && x1 > xmax)) continue;

		float dx = x1 - x0, dyv = y1 - y0;
		float t0 = 0.f, t1 = 1.f;

		if (x0 < xmin) t0 = (xmin - x0) / dx;
		if (x1 < xmin) t1 = (xmin - x0) / dx;
		if (x0 > xmax) t0 = (xmax - x0) / dx;
		if (x1 > xmax) t1 = (xmax - x0) / dx;

		if (t0 > t1) std::swap(t0, t1);

		float sx0 = x0 + dx * t0, sy0 = y0 + dyv * t0;
		float sx1 = x0 + dx * t1, sy1 = y0 + dyv * t1;

		std::vector<Coords> seg = { {sx0, sy0}, {sx1, sy1} };
		line.draw(seg, color, std::min(4.f, ww * 0.004f));
	}
}

void Blueprint::drawPulses() {
	Color color(255, 255, 255);
	float ww = window.getWidth();
	float wh = window.getHeight();
	float width = ww * 0.28f;
	float height = ww * 0.04f;
	float winWidth = ww - margin * 2;
	float winHeight = wh - margin - top;
	float y = (top + winHeight - height) / wh;
	float x = (margin + winWidth * 0.5f - width * 0.5f) / ww;
	float size = std::max(0.0012f * std::sqrt(height), 0.03f);

	if (startingPulse != 1 && currentlyDrawing)
		pulse.draw(x, y, size, color, 0.48, 0.32, std::max(320 / std::pow(height, 0.01f), 360.f), hoveringPulse == 1 ? 1.75 : 1);
	if (startingPulse != 2 && currentlyDrawing)
		pulse.draw(x + 0.28f, y, size, color, 0.44, 0.32, std::max(320 / std::pow(height, 0.01f), 360.f), hoveringPulse == 2 ? 1.75 : 1);
}

int Blueprint::insidePulse(float mx, float my) {
	float ww = window.getWidth();
	float wh = window.getHeight();

	float width = ww * 0.28f;
	float height = ww * 0.04f;

	float winWidth = ww - margin * 2;
	float winHeight = wh - margin - top;

	float px = margin + winWidth * 0.5f - width * 0.5f;
	float py = wh - (top + winHeight - height);

	float nx = mx / ww;
	float ny = my / wh;

	float cx1 = px / ww;
	float cy1 = py / wh;
	float cx2 = cx1 + 0.28f;

	float size = std::max(0.0012f * std::sqrt(height), 0.03f) * 0.75f;

	float dx1 = nx - cx1;
	float dy1 = ny - cy1;
	float dx2 = nx - cx2;
	float dy2 = ny - cy1;

	float aspect = ww / wh;
	dy1 /= aspect;
	dy2 /= aspect;

	float d1 = std::sqrt(dx1 * dx1 + dy1 * dy1);
	float d2 = std::sqrt(dx2 * dx2 + dy2 * dy2);

	if (d1 <= size)
		return 1;
	else if (d2 <= size)
		return 2;

	return 0;
}


bool Blueprint::insideBorder(float x, float y) {
	float width = window.getWidth() - margin * 2;
	float height = window.getHeight() - margin - top;
	return x >= margin && x <= margin + width && y >= margin && y <= margin + height;
}

Coords Blueprint::convertToLocal(float mx, float my) {
	float bw = window.getWidth() - margin * 2;
	float bh = window.getHeight() - margin - top;

	float localXpx = mx - margin;
	float localYpx = my - margin;

	float x = localXpx / bw;
	float unitSize = bw / 1.0f;
	float y = localYpx / unitSize;

	return Coords(x, y);
}

void Blueprint::mouseCallback(double x, double y, int button, int action, int mods) {
	float mx = float(x), my = float(window.getHeight() - y);

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && currentlyDrawing && drawing.empty() && hoveringPulse && insideBorder(mx, my)) {
		drawing.push_back(convertToLocal(mx, my));
		startingPulse = hoveringPulse;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE && !drawing.empty()) {
		if (hoveringPulse == 3 - startingPulse)
			currentlyDrawing = false;
		else
			drawing.clear();
		startingPulse = 0;
	}
}

Coords Blueprint::clipToBorder(float mx, float my) {
	float xmin = margin;
	float xmax = window.getWidth() - margin;
	float ymin = margin;
	float ymax = window.getHeight() - top;

	if (mx < xmin) mx = xmin;
	if (mx > xmax) mx = xmax;
	if (my < ymin) my = ymin;
	if (my > ymax) my = ymax;

	return convertToLocal(mx, my);
}

void Blueprint::cursorCallback(double x, double y) {
	float my = float(window.getHeight() - y);
	float mx = float(x);

	hoveringPulse = insidePulse(mx, my);

	if (!currentlyDrawing) return;
	if (drawing.empty()) return;

	Coords p = clipToBorder(mx, my);
	drawing.push_back(p);
}

void Blueprint::drawDrawing() {
	if (drawing.size() < 2) return;

	float bw = window.getWidth() - margin * 2;
	float bh = window.getHeight() - margin - top;

	float unit = bw;

	std::vector<Coords> pts;
	pts.reserve(drawing.size());

	for (auto& p : drawing) {
		float x = margin + p.x * bw;
		float y = std::max(top, window.getHeight() - (margin + p.y * unit));
		pts.push_back(Coords(x, y));
	}

	Color c(255, 255, 255);
	line.draw(pts, c, 6);
}

void Blueprint::draw() {
	drawMesh();
	drawPlatform();
	drawBorder();
	drawPulses();
	drawDrawing();
	trash.draw();
}