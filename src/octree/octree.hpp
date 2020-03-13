#pragma once

#include <glm/glm.hpp>

#include "util/gl.hpp"

struct Octree
{
	const size_t max_resolution;

	const GLuint texture_name;
	const GLuint buffer_name;

	Octree(size_t max_resolution);
	~Octree();

	void bind(GLuint binding);

	static size_t size(glm::uint level);
};
