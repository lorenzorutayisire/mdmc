#include "Shader.hpp"

#include <fstream>
#include <string>

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
	std::ifstream in(path);
	std::string string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

	this->source_from_string(string.c_str());
}

bool Shader::compile()
{
	glCompileShader(this->id);

	GLint status;
	glGetShaderiv(this->id, GL_COMPILE_STATUS, &status);
	return status == GL_FALSE;
}

std::string&& Shader::get_compile_log()
{
	std::string log;
	glGetShaderInfoLog(this->id, 2048, NULL, &log[0]);
	return std::move(log);
}
