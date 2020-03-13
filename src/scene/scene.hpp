#pragma once

#include <vector>
#include <memory>

#include "mesh.hpp"

struct Scene
{
	std::vector<std::shared_ptr<Mesh>> meshes;
};
