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

	void clear(std::shared_ptr<Octree> const& octree, unsigned int start, unsigned int count);

public:
	OctreeBuilder();

	void build(std::shared_ptr<VoxelList> const& voxel_list, std::shared_ptr<Octree> const& octree);
};
