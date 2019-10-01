#pragma once

#include "Shader.hpp"

#include <GL/glew.h>

#include <string>

class Program
{
	GLuint id;

public:
	Program();
	~Program();

	void attach(Shader shader);
	void detach(Shader shader);

	bool link();
	void use();

	std::string get_log();

	GLint get_attrib_location(const GLchar* name);

	GLint get_uniform_location(const GLchar* name);
};
