#pragma once

#include <GL/glew.h>

#include "util/gl.hpp"

struct VoxelList
{
	TextureBuffer position_buffer, color_buffer;
	size_t size;

	VoxelList(size_t size);

	void bind(GLuint position_binding, GLuint color_binding);
};
