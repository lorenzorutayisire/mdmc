#pragma once

#include <glm/glm.hpp>

#include "octree/voxel_list.hpp"

#include "scene.hpp"

#include "util/gl.hpp"

class SceneVoxelizer
{
private:
	Program program;

public:
	SceneVoxelizer();

	std::shared_ptr<VoxelList> voxelize(const glm::uvec3& grid, const Scene& scene);
};
