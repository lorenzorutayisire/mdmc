#pragma once

#include "Shader.hpp"

#include <GL/glew.h>

class Program
{
	GLuint id;

public:
	Program();
	~Program();

	void attach(Shader shader);
	void detach(Shader shader);

	void link();
	void use();

	GLint get_attrib_location(const GLchar* name);

	GLint get_uniform_location(const GLchar* name);
};
