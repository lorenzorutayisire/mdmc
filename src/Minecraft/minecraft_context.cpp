#include "minecraft_context.hpp"

#include <stb_image.h>

void MinecraftContext::build_atlas_texture()
{
	auto atlas = this->assets->atlas;

	glBindTexture(GL_TEXTURE_2D, this->atlas_texture);

	int width, height;
	stbi_uc* image = stbi_load(atlas->image_path.u8string().c_str(), &width, &height, nullptr, STBI_rgb_alpha);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	stbi_image_free(image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
}

MinecraftContext::MinecraftContext(std::shared_ptr<MinecraftAssets const> const& assets) :
	assets(assets)
{
	glGenTextures(1, &this->atlas_texture);

	this->build_atlas_texture();
}

MinecraftContext::~MinecraftContext()
{
	glDeleteTextures(1, &this->atlas_texture);
}

GLuint MinecraftContext::get_atlas_texture() const
{
	return this->atlas_texture;
}

std::shared_ptr<MinecraftContext const> MinecraftContext::build(std::shared_ptr<MinecraftAssets const> const& assets)
{
	return std::make_shared<MinecraftContext const>(assets);
}
