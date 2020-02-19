#pragma once

#include "gl.hpp"

struct Volume
{
	const uvec3 size;
	const Texture3d texture3d;

	Volume(uvec3 size, GLuint texture3d);
	Volume(const Volume&) = delete;

	~Volume();
};
