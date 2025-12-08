#include "Button.h"
#include "WindowManager.h"
#include "InputListener.h"
#include "DataClasses.h"
#include "GLFW/glfw3.h"
#include "GL/glew.h"

Button::Button(WindowManager& window, Bounds bounds, Color background, Color edge, float darken, float resolution) :
	bounds(bounds), background(background), edge(edge), darken(darken), resolution(resolution),
	window(window), shader("shaders/button.vert", "shaders/button.frag") {
	float quad[] = {
		0, 0,
		1, 0,
		1, 1,
		0, 1
	};

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	window.addMouseListener(this);
}

void Button::setListener(ButtonListener* listener) {
	this->listener = listener;
}

bool Button::hit(float px, float py) const {
	return px >= bounds.x &&
		px <= bounds.x + bounds.width &&
		py >= bounds.y &&
		py <= bounds.y + bounds.width;
}

void Button::mouseCallback(double x, double y, int button, int action, int mods) {
	if (button != GLFW_MOUSE_BUTTON_LEFT) return;

	float winW = window.getWidth();
	float winH = window.getHeight();
	float px = float(winW - x);
	float py = float(y);
	bool inside = hit(px, py);

	if (action == GLFW_PRESS) {
		pressed = inside;
		return;
	}

	if (action == GLFW_RELEASE) {
		if (pressed && inside) {
			selected ^= 1;
			if (listener) listener->buttonChanged(*this, selected);
		}
		pressed = false;
	}
}

void Button::draw() {
	shader.use();

	float shade = pressed ? (1.0f - darken) : 1.0f;
	float winW = window.getWidth();
	float winH = window.getHeight();

	float ns = bounds.width / winW;
	float nh = bounds.width / winH;

	float nx = (winW - bounds.x) / winW - ns;
	float ny = 1.0f - (bounds.y / winH) - nh;

	shader.setVec3("uBg", background.red * shade, background.green * shade, background.blue * shade);
	shader.setVec3("uEdge", edge.red, edge.green, edge.blue);
	shader.setVec2("uSize", ns, nh);
	shader.setVec2("uPos", nx, ny);
	shader.setFloat("uPix", resolution);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

