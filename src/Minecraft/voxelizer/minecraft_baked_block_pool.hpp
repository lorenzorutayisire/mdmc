#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

#include <GL/glew.h>

#include "util/gl.hpp"
#include "minecraft/assets/minecraft_assets.hpp"

class MinecraftBakedBlockPool;

struct MinecraftBakedBlock
{
	MinecraftBakedBlockPool const* pool;
	unsigned int start_at, count;

	MinecraftBakedBlock(
		MinecraftBakedBlockPool const* pool,
		unsigned int start_at,
		unsigned int end_at
	);

	void draw() const;
};

class MinecraftBakedBlockPool
{
	friend void MinecraftBakedBlock::draw() const;

private:
	std::vector<MinecraftBakedBlock> block_by_id;
	std::unordered_map<std::string, MinecraftBakedBlock> block_by_name;

	GLuint vao;

public:
	GLuint vbo;

	MinecraftBakedBlockPool();
	MinecraftBakedBlockPool(MinecraftBakedBlockPool const&) = delete;
	MinecraftBakedBlockPool(MinecraftBakedBlockPool const&&) = delete;

	~MinecraftBakedBlockPool();

	MinecraftBakedBlock const& get_block(unsigned int block_id) const;
	MinecraftBakedBlock const& get_block(std::string const& block_name) const;

	void bake(std::shared_ptr<MinecraftAssets const> const& assets);
};
