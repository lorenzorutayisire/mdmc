#pragma once

#include <unordered_set>
#include <memory>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include "minecraft_version_pool.hpp"
#include "minecraft_block_state.hpp"

struct MinecraftWorldRenderer
{
private:
	std::shared_ptr<MinecraftVersionPool> version_pool;
	std::unordered_map<glm::uvec3, MinecraftBlockStateVariant*> blocks_by_position; // The existance of MinecraftBlockState* is guaranted by version_pool.

public:
	const GLuint vbo;

	MinecraftWorldRenderer(const std::shared_ptr<MinecraftVersionPool>& version_pool);
	~MinecraftWorldRenderer();

	const MinecraftBlockStateVariant* get_block(const glm::uvec3& position) const;
	void set_block(const glm::uvec3& position, const MinecraftBlockStateVariant& block_state_variant, bool must_build = false);
	
	void build();


};
