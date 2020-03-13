#include "texture_packer_atlas_loader.hpp"

#include <fstream>

#include <stb_image.h>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

void load_texture(const std::shared_ptr<Atlas>& atlas, const std::filesystem::path& path)
{
	glBindTexture(GL_TEXTURE_2D, atlas->name);

	int width, height;
	stbi_uc* image = stbi_load(path.u8string().c_str(), &width, &height, nullptr, STBI_rgb_alpha);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	stbi_image_free(image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void load_descriptor(const std::shared_ptr<Atlas>& atlas, const std::filesystem::path& path)
{
	atlas->texture_by_name.clear();

	rapidjson::Document document;

	std::ifstream stream(path);
	rapidjson::IStreamWrapper stream_wrapper(stream);
	document.ParseStream(stream_wrapper);

	auto size = document.GetObject()["meta"].GetObject()["size"].GetObject();
	atlas->width = size["w"].GetInt();
	atlas->height = size["h"].GetInt();

	for (auto& entry : document.GetObject()["frames"].GetArray())
	{
		auto& frame = entry["frame"].GetObject();

		std::string filename = entry["filename"].GetString();
		auto texture_name = "block/" + filename.substr(0, filename.find('.'));

		atlas->texture_by_name.insert(std::make_pair(
			texture_name,
			Atlas::Texture{
				frame["x"].GetInt(), frame["y"].GetInt(),
				frame["w"].GetInt(), frame["h"].GetInt()
			}
		));
	}
}

std::shared_ptr<const Atlas> TexturePackerAtlasLoader::load(const std::filesystem::path& texture_path, const std::filesystem::path& descriptor_path)
{
	std::shared_ptr<Atlas> atlas;
	load_texture(atlas, texture_path);
	load_descriptor(atlas, descriptor_path);
	return atlas;
}
