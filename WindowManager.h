#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>

using ResizeCallback = void(*)();

class WindowManager {
	int width, height, xPos, yPos, lastWidth, lastHeight;
	std::vector<GLFWmousebuttonfun> mouseListeners;
	std::vector<GLFWkeyfun> keyboardListeners;
	ResizeCallback resizeCallbackPtr;
	GLFWwindow* window;
	bool fullscreen;

	static WindowManager& getWindowManager(GLFWwindow* window);

	static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseCallback(GLFWwindow* window, int button, int action, int mods);
	static void resizeCallback(GLFWwindow* window, int newWidth, int newHeight);

	static void switchFullscreen(GLFWwindow* window, int key, int scancode, int action, int mods);
	GLFWmonitor* getMonitor();

public:
	WindowManager(int width, int height, const std::string& title, bool fullscreen);
	void addMouseListener(GLFWmousebuttonfun listener);
	void addKeyboardListener(GLFWkeyfun listener);
	void setResizeCallback(ResizeCallback callback);
	void setIcon(const std::string& iconPath);
	void setFullscreen(bool fullscreen);
	int getHeight() const;
	int getWidth() const;
	bool shouldClose();
	void swapBuffers();
	~WindowManager();
};

