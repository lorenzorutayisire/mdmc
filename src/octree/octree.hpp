#pragma once

#include <glm/glm.hpp>

#include "util/gl.hpp"

struct Octree
{
	const unsigned int resolution;
	const size_t size;
	const size_t capacity;

	TextureBuffer buffer;

	Octree(unsigned int resolution);

	void bind(GLuint binding);
};
