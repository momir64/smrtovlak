#pragma once
#include <GLFW/glfw3.h>

class Button;

class MouseListener {
public:
	virtual void mouseCallback(double x, double y, int button, int action, int mods) = 0;
	virtual void cursorCallback(double x, double y) {};
};

class KeyboardListener {
public:
	virtual void keyboardCallback(GLFWwindow& window, int key, int scancode, int action, int mods) = 0;
};

class ResizeListener {
public:
	virtual void resizeCallback(GLFWwindow& window) = 0;
};

class ButtonListener {
public:
	virtual void buttonChanged(Button& btn, int selected) = 0;
};