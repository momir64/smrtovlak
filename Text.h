#pragma once
#include "WindowManager.h"
#include "Shader.h"
#include <GL/glew.h>
#include <cstdint>
#include <string>
#include <vector>
#include <map>

class Text {
    struct Glyph {
        int bearingX, bearingY;
        int advance;
        GLuint tex;
        int w, h;
    };

    std::vector<uint32_t> utf16_decode(const std::wstring& s);

    std::map<uint32_t, Glyph> glyphs;
    GLuint vao = 0, vbo = 0;
    WindowManager& window;
    Shader shader;

public:
    Text(WindowManager& window);
    void loadFont(const std::string& path, int pixelSize);
    void draw(const std::wstring& text, float x, float y, float sizeRelW);
};
