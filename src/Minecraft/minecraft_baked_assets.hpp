#pragma once

#include <memory>
#include <vector>

#include <GL/glew.h>

#include "util/gl.hpp"
#include "assets/minecraft_assets.hpp"

class MinecraftBakedAssets
{
private:
	using BufferPosition = std::tuple<unsigned int, unsigned int>;

	std::vector<BufferPosition> buffer_position_by_id;
	std::unordered_map<std::string, BufferPosition> buffer_position_by_name;

	GLuint vao;
	GLuint vbo;

	void draw(BufferPosition const& buffer_position) const;

public:
	MinecraftBakedAssets();
	MinecraftBakedAssets(MinecraftBakedAssets const&) = delete;
	MinecraftBakedAssets(MinecraftBakedAssets const&&) = delete;

	~MinecraftBakedAssets();

	void draw(unsigned int block_id) const;
	void draw(std::string const& block_name) const;

	static std::shared_ptr<MinecraftBakedAssets const> bake(std::shared_ptr<MinecraftAssets const> const& assets);
};
