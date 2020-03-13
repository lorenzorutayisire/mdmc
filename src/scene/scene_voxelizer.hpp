#pragma once

#include <glm/glm.hpp>

#include "voxel/voxelizer.hpp"
#include "voxel/voxel_fragment_list.hpp"

#include "scene.hpp"

#include "util/gl.hpp"

class SceneVoxelizer
{
private:
	Program program;

public:
	SceneVoxelizer();

	std::shared_ptr<VoxelFragmentList> voxelize(const glm::uvec3& grid, const Scene& scene);
};
