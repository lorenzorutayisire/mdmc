#pragma once

#include "minecraft_baked_block_pool.hpp"
#include "octree/voxel_list.hpp"

class MinecraftBlockVoxelizer
{
private:
	Program program;

public:
	MinecraftBlockVoxelizer();

	std::shared_ptr<VoxelList> voxelize(MinecraftBakedBlock const& block, unsigned int resolution);
};
