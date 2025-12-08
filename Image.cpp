#include "Image.h"
#include <string>
#include "GL/glew.h"
#include "stb_image.h"
#include "WindowManager.h"

Image::Image(WindowManager& window, const std::string& path) : window(window), shader("shaders/image.vert", "shaders/image.frag") {
	stbi_set_flip_vertically_on_load(true);
	unsigned char* pixels = stbi_load(path.c_str(), &imgW, &imgH, nullptr, 4);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgW, imgH, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_image_free(pixels);

	float quad[] = {
		0, 0,
		1, 0,
		1, 1,
		0, 1,
	};

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

Image::~Image() {
	glDeleteTextures(1, &tex);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void Image::draw(float posX, float posY, float width) {
	float height = width * (float(imgH) / float(imgW)) * (float(window.getWidth()) / float(window.getHeight()));

	shader.use();
	shader.setVec2("uPos", posX, posY);
	shader.setVec2("uSize", width, height);
	shader.setInt("tex", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
