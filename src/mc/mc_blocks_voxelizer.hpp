#pragma once

#include <memory>

#include "mc_assets.hpp"
#include "octree/voxel_list.hpp"

namespace mdmc
{
	// ------------------------------------------------------------------------------------------------ baked_mc_blocks

	struct baked_mc_blocks
	{
		struct baked_block
		{
			std::reference_wrapper<mdmc::baked_mc_blocks> m_pool;
			uint32_t m_start_at, m_count;

			inline baked_block(mdmc::baked_mc_blocks pool) :
				m_pool(pool)
			{}
		};

		GLuint m_atlas_texture;
		GLuint m_vbo;

		std::vector<baked_block> m_blocks;

		baked_mc_blocks();
		~baked_mc_blocks();

		void from_mc_assets(mdmc::mc_assets const& assets);
	};

	// ------------------------------------------------------------------------------------------------ mc_blocks_voxelizer

	struct mc_blocks_voxelizer
	{
		Program m_program;

		mc_blocks_voxelizer();

		std::shared_ptr<VoxelList> voxelize(mdmc::baked_mc_blocks::baked_block const& block, uint32_t volume_side);
	};
}

