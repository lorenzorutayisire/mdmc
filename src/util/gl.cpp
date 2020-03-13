#include "gl.hpp"

#include <fstream>
#include <vector>

// ================================================================================================
// Shader
// ================================================================================================

Shader::Shader(GLenum type) :
	id(glCreateShader(type))
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

// ================================================================================================
// Program
// ================================================================================================

Program::Program() :
	id(glCreateProgram())
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

void Program::unuse()
{
	glUseProgram(0);
}

std::string Program::get_log()
{
	GLint log_length = 0;
	glGetProgramiv(this->id, GL_INFO_LOG_LENGTH, &log_length);

	std::vector<GLchar> log(log_length);
	glGetProgramInfoLog(this->id, log_length, NULL, log.data());

	return std::string(log.begin(), log.end());
}

GLint Program::get_attrib_location(const GLchar* name, GLboolean safe)
{
	GLint loc = glGetAttribLocation(this->id, name);
	if (safe && loc < 0)
		throw std::runtime_error("Attrib location is negative.");
}

GLint Program::get_uniform_location(const GLchar* name, GLboolean safe)
{
	GLint loc = glGetUniformLocation(this->id, name);
	if (safe && loc < 0)
		throw std::runtime_error("Uniform location is negative.");
	return loc;

}

// ================================================================================================
// TextureBuffer
// ================================================================================================

TextureBuffer::TextureBuffer() : 
	buffer_name([]() {
		GLuint name;
		glGenBuffers(1, &name);
		return name;
	}()),
	texture_name([]() {
		GLuint name;
		glGenTextures(1, &name);
		return name;
	}())
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

// ================================================================================================
// AtomicCounter
// ================================================================================================

AtomicCounter::AtomicCounter() :
	name([]() {
		GLuint name;
		glGenBuffers(1, &name);
		return name;
	}())
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

// ================================================================================================
// SSBO
// ================================================================================================

ShaderStorageBuffer::ShaderStorageBuffer() :
	name([]() {
		GLuint name;
		glGenBuffers(1, &name);
		return name;
	}())
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

// ================================================================================================
// FrameBuffer
// ================================================================================================

FrameBuffer::FrameBuffer() :
	name([]() {
		GLuint res;
		glGenFramebuffers(1, &res);
		return res;
	}())
{}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &this->name);
}

void FrameBuffer::set_default_size(GLuint width, GLuint height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->name);

	glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH, width);
	glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::use()
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->name);
}

void FrameBuffer::unuse()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// ================================================================================================
// Texture3d
// ================================================================================================

Texture3d::Texture3d() :
	name([]() {
		GLuint res;
		glGenTextures(1, &res);
		return res;
	}())
{}

Texture3d::~Texture3d()
{
	glDeleteTextures(1, &this->name);
}

void Texture3d::bind() const
{
	glBindTexture(GL_TEXTURE_3D, this->name);
}

void Texture3d::unbind()
{
	glBindTexture(GL_TEXTURE_3D, 0);
}

void Texture3d::set_storage(GLenum format, GLuint width, GLuint height, GLuint depth) const
{
	this->bind();

	glTexStorage3D(GL_TEXTURE_3D, 1, format, width, height, depth);

	this->unbind();
}
