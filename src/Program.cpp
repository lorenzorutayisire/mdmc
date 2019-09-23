#include "Program.hpp"

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

void Program::link()
{
	glLinkProgram(this->id);
}

void Program::use()
{
	glUseProgram(this->id);
}

GLint Program::get_attrib_location(const GLchar* name)
{
	return glGetAttribLocation(this->id, name);
}

GLint Program::get_uniform_location(const GLchar* name)
{
	return glGetUniformLocation(this->id, name);
}
