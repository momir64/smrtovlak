#include "Windows.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "WindowManager.h"
#include "Image.h"
#include "Text.h"

WindowManager window(1000, 800, "Smrtovlak", false);
Image background(window, "assets/background.png");
Text tr(window);

void static draw() {
	glClear(GL_COLOR_BUFFER_BIT);

	background.draw(0.f, 0.f, 1.f);
	tr.draw(L"Momir Stanišić SV39/2022", 20.f, 40.f, 16.0f);

	window.swapBuffers();
	glfwPollEvents();
}

int static run() {
	glClearColor(69 / 255.f, 164 / 255.f, 212 / 255.f, 1.0f);
	window.setIcon("assets/icons/simulation.png");
	tr.loadFont("assets/fonts/jersey.ttf", 256);
	window.setResizeCallback(draw);

	while (!window.shouldClose()) {
		draw();
	}

	return 0;
}


#ifdef _DEBUG
int main() {
	return run();
}
#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	return run();
}
#endif