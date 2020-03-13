#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>

struct Volume
{
	const glm::uvec3 size;
	const GLenum format;

	const GLuint texture3d;

	Volume(glm::uvec3 size, GLenum format);
	~Volume();
};
