#pragma once

#include "mc_assets.hpp"

#define MDMC_VERTEX_SIZE (3 + 2 + 2 + 1)

namespace mdmc
{
	void bake_mc_atlas(GLuint& texture, mdmc::mc_atlas const& atlas);

	size_t bake_mc_block_state_variant(
		mdmc::mc_assets const& assets,
		mdmc::mc_block_state_variant const& block_state_variant,
		glm::mat4 transform,
		std::vector<float>& buffer
	);
}
