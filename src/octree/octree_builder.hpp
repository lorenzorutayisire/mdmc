#pragma once

#include <memory>

#include "util/gl.hpp"

#include "voxel/voxel_fragment_list.hpp"
#include "octree.hpp"

class OctreeBuilder
{
private:
	Program
		node_flag,
		node_alloc,
		node_init,
		store_leaf;

public:
	OctreeBuilder();

	std::shared_ptr<Octree> build(const std::shared_ptr<VoxelFragmentList>& voxel_list, size_t octree_max_resolution);
};
