#pragma once

#include <glm/glm.hpp>

#include "util/gl.hpp"

struct Octree
{
	const size_t max_resolution;
	TextureBuffer buffer;

	Octree(size_t max_resolution);

	void bind(GLuint binding);

	static size_t size(glm::uint level);
};
