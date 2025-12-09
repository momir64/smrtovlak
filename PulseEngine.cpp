#include "PulseEngine.h"
#include "WindowManager.h"
#include "DataClasses.h"
#include "GLFW/glfw3.h"

PulseEngine::PulseEngine(WindowManager& window) :
	window(window), shader("shaders/pulse.vert", "shaders/pulse.frag") {
	float quad[] = { 0,0,  1,0,  1,1,  0,1 };

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
}

void PulseEngine::draw(float x, float y, float size, const Color& col, float speed, float strength, float resolution, float opacity) {
	shader.use();

	shader.setFloat("uAspect", float(window.getWidth()) / float(window.getHeight()));
	shader.setVec3("uColor", col.red, col.green, col.blue);
	shader.setFloat("uTime", glfwGetTime());
	shader.setFloat("uStrength", strength);
	shader.setFloat("uOpacity", opacity);
	shader.setFloat("uPix", resolution);
	shader.setFloat("uSpeed", speed);
	shader.setFloat("uSize", size);
	shader.setVec2("uPos", x, y);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
