#pragma once
#include "Shader.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "DataClasses.h"
#include "InputListener.h"
#include "WindowManager.h"

class Button : public MouseListener {
	WindowManager& window;
	Color background;
	float resolution;
	Bounds bounds;
	Shader shader;
	float darken;
	Color edge;

	bool pressed = false;
	int selected = 0;

	GLuint vao = 0, vbo = 0;
	ButtonListener* listener = nullptr;

	bool hit(float mx, float my) const;

public:
	Button(WindowManager& window, Bounds bounds, Color background, Color edge, float darken, float resolution);
	void mouseCallback(double x, double y, int button, int action, int mods) override;
	void setListener(ButtonListener* listener);
	void draw();
};
