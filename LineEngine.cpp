#include "LineEngine.h"
#include "WindowManager.h"
#include "DataClasses.h"
#include <vector>
#include <cmath>

LineEngine::LineEngine(WindowManager& window)
	: window(window), shader("shaders/line.vert", "shaders/line.frag") {
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
}

void LineEngine::draw(const std::vector<Coords>& points, const Color& color, float width) {
	if (points.size() < 2 || width <= 0.0f) return;

	std::vector<Coords> pts;
	pts.reserve(points.size());

	float halfW = width * 0.5f;
	float minDist2 = halfW * halfW;

	Coords last = points[0];
	pts.push_back(last);
	for (size_t i = 1; i + 1 < points.size(); ++i) {
		float dx = points[i].x - last.x;
		float dy = points[i].y - last.y;
		if (dx * dx + dy * dy >= minDist2) {
			pts.push_back(points[i]);
			last = points[i];
		}
	}
	pts.push_back(points.back());

	if (pts.size() < 2) return;

	if (pts.size() > 5) {
		std::vector<Coords> tmp(pts.size());
		for (int iter = 0; iter < 2; ++iter) {
			tmp[0] = pts[0];
			tmp[pts.size() - 1] = pts[pts.size() - 1];
			for (size_t i = 1; i + 1 < pts.size(); ++i) {
				tmp[i].x = (pts[i - 1].x + 2.0f * pts[i].x + pts[i + 1].x) * 0.25f;
				tmp[i].y = (pts[i - 1].y + 2.0f * pts[i].y + pts[i + 1].y) * 0.25f;
			}
			pts.swap(tmp);
		}
	}

	size_t n = pts.size();
	bool closed = false;
	if (n > 2) {
		float dx = pts.front().x - pts.back().x;
		float dy = pts.front().y - pts.back().y;
		if (dx * dx + dy * dy < 0.25f) closed = true;
	}

	size_t count = closed ? (n - 1) : n;

	std::vector<Coords> vertices;
	vertices.reserve(count * 2 + 4);

	for (size_t i = 0; i < count; ++i) {
		const Coords& p = pts[i];

		Coords prev, next;
		if (!closed && i == 0) {
			prev = p;
			next = pts[i + 1];
		} else if (!closed && i == count - 1) {
			prev = pts[i - 1];
			next = p;
		} else {
			size_t iPrev = (i == 0) ? (count - 1) : (i - 1);
			size_t iNext = (i + 1 == count) ? 0 : (i + 1);
			prev = pts[iPrev];
			next = pts[iNext];
		}

		float dx1, dy1, dx2, dy2;
		if (!closed && i == 0) {
			dx1 = next.x - p.x;
			dy1 = next.y - p.y;
			dx2 = dx1;
			dy2 = dy1;
		} else if (!closed && i == count - 1) {
			dx1 = p.x - prev.x;
			dy1 = p.y - prev.y;
			dx2 = dx1;
			dy2 = dy1;
		} else {
			dx1 = p.x - prev.x;
			dy1 = p.y - prev.y;
			dx2 = next.x - p.x;
			dy2 = next.y - p.y;
		}

		float len1 = std::sqrt(dx1 * dx1 + dy1 * dy1);
		float len2 = std::sqrt(dx2 * dx2 + dy2 * dy2);
		if (len1 == 0.0f && len2 == 0.0f) continue;
		if (len1 == 0.0f) { dx1 = dx2; dy1 = dy2; len1 = len2; }
		if (len2 == 0.0f) { dx2 = dx1; dy2 = dy1; len2 = len1; }

		float ux1 = dx1 / len1;
		float uy1 = dy1 / len1;
		float ux2 = dx2 / len2;
		float uy2 = dy2 / len2;

		float nx1 = -uy1;
		float ny1 = ux1;
		float nx2 = -uy2;
		float ny2 = ux2;

		float mx = nx1 + nx2;
		float my = ny1 + ny2;
		float mLen = std::sqrt(mx * mx + my * my);

		float offX, offY;
		if (mLen < 1e-3f) {
			offX = nx1 * halfW;
			offY = ny1 * halfW;
		} else {
			mx /= mLen;
			my /= mLen;
			float dot = mx * nx1 + my * ny1;
			if (dot > 1.0f) dot = 1.0f;
			if (dot < -1.0f) dot = -1.0f;

			if (std::fabs(dot) < 0.3f) {
				offX = nx1 * halfW;
				offY = ny1 * halfW;
			} else {
				float miterLen = halfW / dot;
				float maxMiter = halfW * 4.0f;
				if (miterLen > maxMiter) miterLen = maxMiter;
				if (miterLen < -maxMiter) miterLen = -maxMiter;
				offX = mx * miterLen;
				offY = my * miterLen;
			}
		}

		Coords vL, vR;
		vL.x = p.x + offX;
		vL.y = p.y + offY;
		vR.x = p.x - offX;
		vR.y = p.y - offY;

		vertices.push_back(vL);
		vertices.push_back(vR);
	}

	if (vertices.size() < 4) return;

	if (closed) {
		vertices.push_back(vertices[0]);
		vertices.push_back(vertices[1]);
	}

	shader.use();
	shader.setVec2("uRes", float(window.getWidth()), float(window.getHeight()));
	shader.setVec3("uColor", color.red, color.green, color.blue);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Coords), vertices.data(), GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Coords), (void*)0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, GLsizei(vertices.size()));
}
