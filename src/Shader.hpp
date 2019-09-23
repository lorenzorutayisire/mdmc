#pragma once

#include <GL/glew.h>

#include <string>

class Shader
{
private:
	GLuint id;

public:
	Shader(GLenum type);
	~Shader();

	GLuint get_id();

	void source_from_string(const GLchar* source);
	void source_from_file(const GLchar* path);

	bool compile();
	std::string&& get_compile_log();
};
