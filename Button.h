#pragma once
#include "Shader.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "DataClasses.h"
#include "InputListener.h"
#include "WindowManager.h"
#include "Image.h"
#include <vector>
#include <string>

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

	std::vector<Image> images;

	bool hit(float mx, float my) const;

public:
	Button(WindowManager& window, Bounds bounds, Color background, Color edge,
		float darken, float resolution, const std::vector<std::string>& imagePaths);

	void mouseCallback(double x, double y, int button, int action, int mods) override;
	void setListener(ButtonListener* listener);
	int getSelected() const;
	void draw();
};
