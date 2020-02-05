#pragma once

#include <GL/glew.h>

#include <string>

// ================================================================================================

struct Shader
{
	const GLuint id;

	Shader(GLuint id);
	Shader(const Shader&) = delete;

	~Shader();

	void source_from_string(const GLchar* source);
	void source_from_file(const GLchar* path);

	bool compile();
	std::string get_log();

	static Shader create(GLenum type);
};

// ================================================================================================

struct Program
{
	const GLuint id;

	Program(GLuint id);
	Program(const Program&) = delete;
	
	~Program();

	void attach_shader(const Shader& shader);
	void detach_shader(const Shader& shader);

	bool link();
	void use();

	std::string get_log();

	GLint get_attrib_location(const GLchar* name);
	GLint get_uniform_location(const GLchar* name);

	static Program create();
};

// ================================================================================================

struct TextureBuffer
{
	const GLuint buffer_name, texture_name;

	TextureBuffer(GLuint buffer_name, GLuint texture_name);
	TextureBuffer(const TextureBuffer&) = delete;

	~TextureBuffer();

	void load_data(GLsizei size, const void* data, GLenum usage);
	void set_format(GLenum format);

	void bind(GLuint binding, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);

	static TextureBuffer create();
};

// ================================================================================================

struct AtomicCounter
{
	const GLuint name;

	AtomicCounter(GLuint name);
	AtomicCounter(const AtomicCounter&) = delete;

	~AtomicCounter();

	GLuint get_value();
	void set_value(GLuint value);

	void bind(GLuint binding);

	static AtomicCounter create();
};

// ================================================================================================

struct ShaderStorageBuffer
{
	const GLuint name;

	ShaderStorageBuffer(GLuint name);
	ShaderStorageBuffer(const ShaderStorageBuffer&) = delete;

	~ShaderStorageBuffer();

	void load_data(GLsizei size, const void* data, GLenum usage) const;

	void bind(GLuint binding) const;

	static ShaderStorageBuffer create();
};

