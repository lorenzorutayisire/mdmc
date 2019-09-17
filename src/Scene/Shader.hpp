#pragma once

#include <GL/glew.h>

#include <iostream>

class Shader
{
public:
	static GLuint loadShader(GLenum type, const GLchar* source)
	{
		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &source, NULL);
		glCompileShader(shader);

		GLint status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) {
			char log[2048];
			glGetShaderInfoLog(shader, 2048, NULL, &log[0]);
			// throw std::runtime_error(log);
			std::cerr << log << std::endl;
			throw;
		}
	}
};
