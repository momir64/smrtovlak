#pragma once
#include <GL/glew.h>
#include "InputListener.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <string>

class WindowManager : public KeyboardListener {
	int width, height, xPos, yPos, lastWidth, lastHeight;
	std::vector<KeyboardListener*> keyboardListeners;
	std::vector<MouseListener*> mouseListeners;
	ResizeListener* resizeListener = nullptr;
	GLFWwindow* window;
	bool fullscreen;

	static WindowManager& getWindowManager(GLFWwindow* window);

	static void keyboardEventHandler(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseEventHandler(GLFWwindow* window, int button, int action, int mods);
	static void resizeEventHandler(GLFWwindow* window, int newWidth, int newHeight);

	GLFWmonitor* getMonitor();

public:
	WindowManager(int width, int height, const std::string& title, const std::string& iconPath, bool fullscreen);
	void keyboardCallback(GLFWwindow& window, int key, int scancode, int action, int mods) override;
	void addKeyboardListener(KeyboardListener* listener);
	void setResizeListener(ResizeListener* listener);
	void addMouseListener(MouseListener* listener);
	void setFullscreen(bool fullscreen);
	int getHeight() const;
	int getWidth() const;
	bool shouldClose();
	void swapBuffers();
	~WindowManager();
};

