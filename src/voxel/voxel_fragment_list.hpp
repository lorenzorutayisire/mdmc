#pragma once

#include <GL/glew.h>

#include "util/gl.hpp"

struct VoxelFragmentList
{
	TextureBuffer voxel_position;
	TextureBuffer voxel_color;

	size_t size;

	void bind(GLuint position_binding, GLuint color_binding);
};
