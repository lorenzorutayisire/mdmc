#pragma once

#include <GL/glew.h>

#include <string>

// ================================================================================================
// Shader
// ================================================================================================

struct Shader
{
	const GLuint id;

	Shader(GLenum type);
	Shader(const Shader&) = delete;

	~Shader();

	void source_from_string(const GLchar* source);
	void source_from_file(const GLchar* path);

	bool compile();
	std::string get_log();
};

// ================================================================================================
// Program
// ================================================================================================

struct Program
{
	const GLuint id;

	Program();
	Program(const Program&) = delete;
	
	~Program();

	void attach_shader(const Shader& shader);
	void detach_shader(const Shader& shader);

	bool link();

	void use();
	static void unuse();

	std::string get_log();

	GLint get_attrib_location(const GLchar* name);
	GLint get_uniform_location(const GLchar* name);
};

// ================================================================================================
// TextureBuffer
// ================================================================================================

struct TextureBuffer
{
	const GLuint buffer_name, texture_name;

	TextureBuffer();
	TextureBuffer(const TextureBuffer&) = delete;

	~TextureBuffer();

	void load_data(GLsizei size, const void* data, GLenum usage);
	void set_format(GLenum format);

	void bind(GLuint binding, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);

	static TextureBuffer create();
};

// ================================================================================================
// AtomicCounter
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
// SSBO
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

// ================================================================================================
// FrameBuffer
// ================================================================================================

struct FrameBuffer
{
	const GLuint name;

	FrameBuffer(GLuint name);
	~FrameBuffer();

	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer(const FrameBuffer&&) = delete;

	void set_default_size(GLuint width, GLuint height);

	void use();
	void unuse();

	static FrameBuffer create();
};

// ================================================================================================
// Texture3d
// ================================================================================================

struct Texture3d
{
private:
	Texture3d(GLuint name);
	
public:
	const GLuint name;

	~Texture3d();

	Texture3d(const Texture3d&) = delete;
	Texture3d(const Texture3d&&) = delete;

	void set_storage(GLenum format, GLuint width, GLuint height, GLuint depth);

	static Texture3d create();
};