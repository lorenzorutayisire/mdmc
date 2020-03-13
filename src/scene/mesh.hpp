#pragma once

#include <array>

#include <GL/glew.h>

#include <glm/glm.hpp>

// ================================================================================================
// Material
// ================================================================================================

class Material
{
public:
	enum Type
	{
		NONE,
		DIFFUSE,
		AMBIENT,
		SPECULAR,
		EMISSIVE,

		size
	};

private:
	glm::vec4 color[Material::Type::size];
	GLuint texture[Material::Type::size];

public:
	Material();
	Material(const Material&) = delete;
	Material(const Material&&) = delete;

	~Material();

	glm::vec4& get_color(Material::Type type) { return color[type]; }
	GLuint get_texture(Material::Type type) { return texture[type]; }
};

// ================================================================================================
// Mesh
// ================================================================================================

struct Mesh
{
	enum Attribute
	{
		POSITION = 0,
		NORMAL = 1,
		UV = 2,
		COLOR = 3,

		size
	};

	const GLuint vao;
	const std::array<GLuint, Mesh::Attribute::size> vbo;

	const GLuint ebo;
	size_t elements_count;

	glm::mat4 transform;

	std::shared_ptr<Material> material;

	Mesh();

	Mesh(const Mesh&) = delete;
	Mesh(const Mesh&&) = delete;

	~Mesh();
};

