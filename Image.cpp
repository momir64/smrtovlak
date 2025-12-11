#include "Image.h"
#include <string>
#include <vector>
#include "GL/glew.h"
#include "stb_image.h"
#include "DataClasses.h"
#include "WindowManager.h"

Image::Image(WindowManager& window, const std::vector<std::string>& paths)
	: window(window), shader("shaders/image.vert", "shaders/image.frag") {

	stbi_set_flip_vertically_on_load(true);

	for (auto& path : paths) {
		int w, h;
		unsigned char* pixels = stbi_load(path.c_str(), &w, &h, nullptr, 4);
		if (!pixels) continue;
		if (imgW == 0) { imgW = w; imgH = h; }

		GLuint tex;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		stbi_image_free(pixels);
		textures.push_back(tex);
	}

	stbi_set_flip_vertically_on_load(false);

	float quad[] = { 0,0, 1,0, 1,1, 0,1 };
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

Image::~Image() {
	for (auto tex : textures) glDeleteTextures(1, &tex);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void Image::draw(Bounds bounds, const std::vector<float>& opacity, bool flipX, float angle, float pivotHeight, bool widthRelative, long long frame) {
	float aspectRatio = float(window.getWidth()) / float(window.getHeight());
	float y = widthRelative ? bounds.y * aspectRatio : bounds.y;
	int frameCount = (imgH > 0) ? (imgW / imgH) : 1;
	bool isSprite = (frame != -1 && frameCount > 0);

	float height = bounds.width * aspectRatio;
	if (!isSprite)
		height *= float(imgH) / float(imgW);

	int index = 0;
	if (isSprite) {
		long long f = frame % frameCount;
		if (f < 0) f += frameCount;
		index = int(f);
	}

	shader.use();
	shader.setVec2("uSize", bounds.width, height);
	shader.setFloat("uPivotHeight", pivotHeight);
	shader.setInt("uSprite", isSprite ? 1 : 0);
	shader.setInt("uFrameCount", frameCount);
	shader.setInt("uFlipX", flipX ? 1 : 0);
	shader.setInt("uFrameIndex", index);
	shader.setVec2("uPos", bounds.x, y);
	shader.setFloat("uAngle", angle);
	shader.setInt("tex", 0);

	glBindVertexArray(vao);

	for (size_t i = 0; i < textures.size() && i < opacity.size(); ++i) {
		float a = opacity[i];
		if (a <= 0.f) continue;
		shader.setFloat("uAlpha", a);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}
}
