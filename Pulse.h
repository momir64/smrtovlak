#pragma once
#include "Shader.h"
#include "WindowManager.h"
#include "DataClasses.h"
#include <GL/glew.h>

class Pulse {
    GLuint vao = 0, vbo = 0;
    WindowManager& window;
    Shader shader;

public:
    Pulse(WindowManager& window);
    void draw(float x, float y, float size, const Color& col, float speed, float strength, float resolution, float opacity);
};
