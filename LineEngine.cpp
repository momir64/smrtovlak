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
    if (points.size() < 2) return;

    std::vector<Coords> out;
    out.reserve(points.size() * 2);

    for (size_t i = 0; i + 1 < points.size(); i++) {
        Coords A2, B2;
        extendSegment(points[i], points[i + 1], width, A2, B2);
        out.push_back(A2);
        out.push_back(B2);
    }

    shader.use();
    shader.setVec2("uRes", float(window.getWidth()), float(window.getHeight()));
    shader.setVec3("uColor", color.red, color.green, color.blue);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, out.size() * sizeof(Coords), out.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Coords), (void*)0);

    glLineWidth(width);

    for (size_t i = 0; i < out.size(); i += 2)
        glDrawArrays(GL_LINES, i, 2);
}

void LineEngine::extendSegment(const Coords& A, const Coords& B, float width, Coords& A2, Coords& B2) {
	float dx = B.x - A.x;
	float dy = B.y - A.y;
	float len = sqrtf(dx * dx + dy * dy);
	if (len == 0) { A2 = A; B2 = B; return; }

	float ux = dx / len;
	float uy = dy / len;
	float ext = width * 0.5f;

	A2.x = A.x - ux * ext;
	A2.y = A.y - uy * ext;
	B2.x = B.x + ux * ext;
	B2.y = B.y + uy * ext;
}