#include "Shader.hpp"

#include <fstream>
#include <string>
#include <vector>

#include <iostream>

Shader::Shader(GLenum type)
{
	this->id = glCreateShader(type);
}

Shader::~Shader()
{
	glDeleteShader(this->id);
}

GLuint Shader::get_id()
{
	return this->id;
}

void Shader::source_from_string(const GLchar* source)
{
	glShaderSource(this->id, 1, &source, nullptr);
}

void Shader::source_from_file(const GLchar* path)
{
	std::ifstream file(path);
	
	if (errno)
	{
		std::cerr << strerror(errno) << std::endl;
		throw;
	}

	std::string string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	this->source_from_string(string.c_str());
}

bool Shader::compile()
{
	glCompileShader(this->id);

	GLint status;
	glGetShaderiv(this->id, GL_COMPILE_STATUS, &status);
	return status == GL_TRUE;
}

std::string Shader::get_log()
{
	GLint log_length = 0;
	glGetShaderiv(this->id, GL_INFO_LOG_LENGTH, &log_length);

	std::vector<GLchar> log(log_length);
	glGetShaderInfoLog(this->id, log_length, NULL, log.data());

	return std::string(log.begin(), log.end());
}
