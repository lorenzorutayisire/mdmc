#pragma once 

#include <memory>

#include <GL/glew.h>

#include "assets/minecraft_assets.hpp"

class MinecraftContext
{
private:
	GLuint atlas_texture;


	void build_atlas_texture();
public:
	std::shared_ptr<MinecraftAssets const> const assets;

	MinecraftContext(std::shared_ptr<MinecraftAssets const> const& assets);
	MinecraftContext(const MinecraftContext&) = delete;
	MinecraftContext(const MinecraftContext&&) = delete;

	~MinecraftContext();

	GLuint get_atlas_texture() const;

	static std::shared_ptr<MinecraftContext const> build(std::shared_ptr<MinecraftAssets const> const& assets);
};
