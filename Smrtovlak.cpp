#include "Smrtovlak.h"
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include "Button.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "DataClasses.h"

Smrtovlak::Smrtovlak() :
	window(1000, 800, "Smrtovlak", "assets/icons/simulation.png", false),
	signature(window, "assets/fonts/jersey.ttf", 256), simulation(window),
	button(window, Bounds(15, 15, 100), Color(183, 198, 215), Color(1.0f, 1.0f, 1.0f),
		0.15f, 16, std::vector<std::string>{ "assets/icons/blueprint.png", "assets/icons/simulation.png"}) {
}

void Smrtovlak::resizeCallback(GLFWwindow& window) {
	draw();
}

void Smrtovlak::draw() {
	glClear(GL_COLOR_BUFFER_BIT);

	if (button.getSelected())
		blueprint.draw();
	else
		simulation.draw();

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

	const double targetFrame = 1.0 / 75.0;
	auto last = std::chrono::high_resolution_clock::now();

	while (!window.shouldClose()) {
		auto start = std::chrono::high_resolution_clock::now();

		draw(); 
		
		auto end = std::chrono::high_resolution_clock::now();
		double remaining = targetFrame - (end - start).count();
		if (remaining > 0) 
			std::this_thread::sleep_for(std::chrono::duration<double>(remaining));
	}

	return 0;
}

void Smrtovlak::buttonChanged(Button& button, int selected) {
	Color background = selected ? Color(10, 84, 153) : Color(69, 164, 212);
	glClearColor(background.red, background.green, background.blue, 1.0f);
}
