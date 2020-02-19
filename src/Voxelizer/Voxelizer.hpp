#pragma once

#include <memory>

#include "gl.hpp"
#include "field.hpp"

#include <glm/glm.hpp>
#include <GL/glew.h>

using namespace glm;

// ================================================================================================

struct Voxelizer
{
private:
	Program program;

public:
	Voxelizer();

	std::shared_ptr<const Volume> voxelize(const std::shared_ptr<const Field>& field, glm::uvec3 grid);
};
