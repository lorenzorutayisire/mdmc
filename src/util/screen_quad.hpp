#pragma once

#include <GL/glew.h>

class ScreenQuad
{
private:
	GLuint vao, vbo;

public:
	ScreenQuad();
	~ScreenQuad();

	void render();
};
