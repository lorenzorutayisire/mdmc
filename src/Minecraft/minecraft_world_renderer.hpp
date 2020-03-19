#pragma once

#include "util/gl.hpp"
#include "minecraft_world.hpp"
#include "minecraft_baked_assets.hpp"

#include <glm/glm.hpp>

class MinecraftWorldRenderer
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
	MinecraftWorldRenderer();

	void render_block(
		glm::mat4 const& camera,
		glm::mat4 const& transform,
		glm::vec4 const& tint,
		std::shared_ptr<MinecraftContext const> const& context,
		std::shared_ptr<MinecraftBakedAssets const> const& baked_assets,
		int block_id
	);

	void render_block(
		glm::mat4 const& camera,
		glm::mat4 const& transform,
		glm::vec4 const& tint,
		std::shared_ptr<MinecraftContext const> const& context,
		std::shared_ptr<MinecraftBakedAssets const> const& baked_assets,
		std::string const& block_name
	);

	void render_world(
		glm::mat4 const& camera,
		glm::mat4 const& transform,
		glm::vec4 const& tint,
		std::shared_ptr<MinecraftContext const> const& context,
		std::shared_ptr<MinecraftWorld> const& world
	);
};
