#include "Program.hpp"

#include <vector>

Program::Program()
{
	this->id = glCreateProgram();
}

Program::~Program()
{
}

void Program::attach(Shader shader)
{
	glAttachShader(this->id, shader.get_id());
}

void Program::detach(Shader shader)
{
	glDetachShader(this->id, shader.get_id());
}

bool Program::link()
{
	glLinkProgram(this->id);

	GLint status;
	glGetProgramiv(this->id, GL_LINK_STATUS, &status);
	return status == GL_TRUE;
}

void Program::use()
{
	glUseProgram(this->id);
}

std::string Program::get_log()
{
	GLint log_length = 0;
	glGetProgramiv(this->id, GL_INFO_LOG_LENGTH, &log_length);

	std::vector<GLchar> log(log_length);
	glGetProgramInfoLog(this->id, log_length, NULL, log.data());

	return std::string(log.begin(), log.end());
}

GLint Program::get_attrib_location(const GLchar* name)
{
	return glGetAttribLocation(this->id, name);
}

GLint Program::get_uniform_location(const GLchar* name)
{
	return glGetUniformLocation(this->id, name);
}
