#include "Smrtovlak.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "DataClasses.h"

Smrtovlak::Smrtovlak() :
	window(1000, 800, "Smrtovlak", "assets/icons/simulation.png", false),
	signature(window, "assets/fonts/jersey.ttf", 256),
	background(window, "assets/background.png"),
	button(window, Bounds(15, 15, 75), Color(169, 177, 209), Color(1.0f, 1.0f, 1.0f), 0.25f, 16) {
}

void Smrtovlak::resizeCallback(GLFWwindow& window) {
	draw();
}

void Smrtovlak::draw() {
	glClear(GL_COLOR_BUFFER_BIT);

	background.draw(Bounds(0.5f, 0.f, 1.f));
	signature.draw(L"Momir Stanišić SV39/2022", Bounds(25, 50, 18));

	button.draw();

	window.swapBuffers();
	glfwPollEvents();
}

int Smrtovlak::run() {
	Color background(69, 164, 212);
	glClearColor(background.red, background.green, background.blue, 1.0f);
	window.setResizeListener(this);
	button.setListener(this);

	while (!window.shouldClose()) {
		draw();
	}

	return 0;
}