#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "voxel.hpp"
#include "voxel_fragment_list.hpp"

template<typename TObject>
class Voxelizer
{
public:
	virtual std::shared_ptr<VoxelFragmentList> voxelize(const glm::uvec3& grid, const TObject& object) = 0;
};

