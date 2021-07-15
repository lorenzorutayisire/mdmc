#pragma once

#ifdef IGNORED
#include "util/gl.hpp"
#include "minecraft_world.hpp"

#include "voxelizer/minecraft_baked_block_pool.hpp"

#include <glm/glm.hpp>

class MinecraftRenderer
{
private:
	Program program;

	void render(
		glm::mat4 const& camera,
		glm::mat4 const& transform,
		glm::vec4 const& tint,
		std::shared_ptr<MinecraftContext const> const& context,
		std::function<void()> const& draw_call
	);

public:
	MinecraftRenderer();

	void render_block(
		glm::mat4 const& camera,
		glm::mat4 const& transform,
		glm::vec4 const& tint,
		std::shared_ptr<MinecraftContext const> const& context,
		MinecraftBakedBlock const& baked_block
	);

	void render_world(
		glm::mat4 const& camera,
		glm::mat4 const& transform,
		glm::vec4 const& tint,
		std::shared_ptr<MinecraftContext const> const& context,
		std::shared_ptr<MinecraftWorld> const& world
	);
};

#endif
