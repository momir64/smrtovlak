#pragma once
#include "WindowManager.h"
#include "InputListener.h"
#include "DataClasses.h"
#include "GLFW/glfw3.h"
#include "LineEngine.h"
#include "Button.h"
#include "Pulse.h"
#include <vector>

class Blueprint : public MouseListener, KeyboardListener, ButtonListener {
	int hoveringPulse, startingPulse;
	bool currentlyDrawing = true;
	std::vector<Coords>& drawing;
	WindowManager& window;
	float top, margin;
	LineEngine line;
	Button trash;
	Pulse pulse;

	Coords convertToLocal(float mx, float my);
	Coords clipToBorder(float mx, float my);
	bool insideBorder(float mx, float my);
	int insidePulse(float mx, float my);

	void drawPlatform();
	void drawDrawing();
	void drawBorder();
	void drawPulses();
	void drawMesh();

public:
	Blueprint(WindowManager& window, std::vector<Coords>& tracks);
	void keyboardCallback(GLFWwindow& window, int key, int scancode, int action, int mods) override;
	void mouseCallback(double x, double y, int button, int action, int mods) override;
	void buttonChanged(Button& button, int selected) override;
	void cursorCallback(double x, double y) override;
	void draw();
};

