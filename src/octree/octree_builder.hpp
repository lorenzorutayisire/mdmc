#pragma once

#include <memory>

#include "util/gl.hpp"

#include "voxel_list.hpp"
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

	std::shared_ptr<Octree> build(std::shared_ptr<VoxelList> const& voxel_list, size_t octree_max_resolution);
};
