#pragma once
#include "WindowManager.h"
#include "Shader.h"
#include <GL/glew.h>
#include <cstdint>
#include <string>
#include <vector>
#include <map>

class TextEngine {
    struct Glyph {
        int bearingX, bearingY;
        int advance;
        GLuint tex;
        int w, h;
    };

    std::vector<uint32_t> utf16_decode(const std::wstring& s);
    void loadFont(const std::string& path, int pixelSize);

    std::map<uint32_t, Glyph> glyphs;
    GLuint vao = 0, vbo = 0;
    WindowManager& window;
    Shader shader;

public:
    TextEngine(WindowManager& window, const std::string& path, int pixelSize);
    void draw(const std::wstring& text, float x, float y, float sizeRelW);
};
