#include "gl.hpp"

#include <fstream>
#include <vector>

// ================================================================================================
// Shader
// ================================================================================================

Shader::Shader(GLuint id) :
	id(id)
{}

Shader::~Shader()
{
	glDeleteShader(this->id);
}

void Shader::source_from_string(const GLchar* source)
{
	glShaderSource(this->id, 1, &source, nullptr);
}

void Shader::source_from_file(const GLchar* path)
{
	std::ifstream file(path);
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

Shader Shader::create(GLenum type)
{
	return Shader(glCreateShader(type));
}

// ================================================================================================
// Program
// ================================================================================================

Program::Program(GLuint id) :
	id(id)
{}

Program::~Program()
{
	glDeleteProgram(this->id);
}

void Program::attach_shader(const Shader& shader)
{
	glAttachShader(this->id, shader.id);
}

void Program::detach_shader(const Shader& shader)
{
	glDetachShader(this->id, shader.id);
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

Program Program::create()
{
	return Program(glCreateProgram());
}

// ================================================================================================
// TextureBuffer
// ================================================================================================

TextureBuffer::TextureBuffer(GLuint buffer_name, GLuint texture_name) :
	buffer_name(buffer_name),
	texture_name(texture_name)
{}

TextureBuffer::~TextureBuffer()
{
	glDeleteBuffers(1, &this->buffer_name);
	glDeleteTextures(1, &this->texture_name);
}

void TextureBuffer::load_data(GLsizei size, const void* data, GLenum usage)
{
	glBindBuffer(GL_TEXTURE_BUFFER, this->buffer_name);
	glBufferData(GL_TEXTURE_BUFFER, size, data, usage);

	glBindBuffer(GL_TEXTURE_BUFFER, 0);
}

void TextureBuffer::set_format(GLenum format)
{
	glBindBuffer(GL_TEXTURE_BUFFER, this->buffer_name);
	glBindTexture(GL_TEXTURE_BUFFER, this->texture_name);

	glTexBuffer(GL_TEXTURE_BUFFER, format, this->buffer_name);

	glBindTexture(GL_TEXTURE_BUFFER, 0);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);
}

void TextureBuffer::bind(GLuint binding, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format)
{
	glBindImageTexture(binding, this->texture_name, level, layered, layer, access, format);
}

TextureBuffer TextureBuffer::create()
{
	GLuint buffer_name, texture_name;
	glGenBuffers(1, &buffer_name);
	glGenTextures(1, &texture_name);

	return TextureBuffer(buffer_name, texture_name);
}

// ================================================================================================
// AtomicCounter
// ================================================================================================

AtomicCounter::AtomicCounter(GLuint name) :
	name(name)
{}

AtomicCounter::~AtomicCounter()
{
	glDeleteBuffers(1, &this->name);
}

GLuint AtomicCounter::get_value()
{
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, this->name);
	GLuint* value = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), GL_MAP_READ_BIT);

	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	return *value;
}

void AtomicCounter::set_value(GLuint value)
{
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, this->name);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), &value, GL_STATIC_DRAW);

	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
}

void AtomicCounter::bind(GLuint binding)
{
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, binding, this->name);
}

AtomicCounter AtomicCounter::create()
{
	GLuint name;
	glGenBuffers(1, &name);
	return AtomicCounter(name);
}

// ================================================================================================
// ShaderStorageBuffer
// ================================================================================================

ShaderStorageBuffer::ShaderStorageBuffer(GLuint name) :
	name(name)
{}


ShaderStorageBuffer::~ShaderStorageBuffer()
{
	glDeleteBuffers(1, &this->name);
}

void ShaderStorageBuffer::load_data(GLsizei size, const void* data, GLenum usage) const
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->name);
	glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, usage);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ShaderStorageBuffer::bind(GLuint binding) const
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, this->name);
}

ShaderStorageBuffer ShaderStorageBuffer::create()
{
	GLuint name;
	glGenBuffers(1, &name);
	return ShaderStorageBuffer(name);
}
