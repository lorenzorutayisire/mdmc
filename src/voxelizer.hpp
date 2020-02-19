#pragma once

#include <memory>

#include "gl.hpp"
#include "field.hpp"

#include <glm/glm.hpp>
#include <GL/glew.h>

using namespace glm;

// ================================================================================================

struct Volume
{
	const uvec3 size;
	const Texture3d texture3d;

	Volume(uvec3 size, GLuint texture3d);
	Volume(const Volume&) = delete;

	~Volume();

	static Volume create(uvec3 size, GLenum format);
};


// ================================================================================================

struct Voxelizer
{
private:
	Program program;

public:
	Voxelizer();

	std::shared_ptr<const Volume> voxelize(const std::shared_ptr<const Field>& field, glm::uvec3 grid);
};
