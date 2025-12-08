#pragma once
#include <GLFW/glfw3.h>

class MouseListener {
public:
	virtual void mouseCallback(GLFWwindow& window, int button, int action, int mods) = 0;
};

class KeyboardListener {
public:
	virtual void keyboardCallback(GLFWwindow& window, int key, int scancode, int action, int mods) = 0;
};

class ResizeListener {
public:
	virtual void resizeCallback(GLFWwindow& window) = 0;
};