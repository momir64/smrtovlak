#include "Smrtovlak.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"

Smrtovlak::Smrtovlak() :
	window(1000, 800, "Smrtovlak", "assets/icons/simulation.png", false),
	signature(window, "assets/fonts/jersey.ttf", 256),
	background(window, "assets/background.png") {
}

void Smrtovlak::resizeCallback(GLFWwindow& window) {
	draw();
}

void Smrtovlak::draw() {
	glClear(GL_COLOR_BUFFER_BIT);

	background.draw(0.5f, 0.f, 1.f);
	signature.draw(L"Momir Stanišić SV39/2022", 20.f, 40.f, 16.0f);

	window.swapBuffers();
	glfwPollEvents();
}

int Smrtovlak::run() {
	glClearColor(69 / 255.f, 164 / 255.f, 212 / 255.f, 1.0f);
	window.setResizeListener(this);

	while (!window.shouldClose()) {
		draw();
	}

	return 0;
}