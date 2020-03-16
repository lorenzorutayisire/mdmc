#include "texture_packer_atlas_loader.hpp"

#include <fstream>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

std::shared_ptr<Atlas const> TexturePackerAtlasLoader::load(std::filesystem::path const& image_path, std::filesystem::path const& descriptor_path)
{
	auto atlas = std::make_shared<Atlas>();
	rapidjson::Document document;

	// Image
	atlas->image_path = image_path;

	// Descriptor
	std::ifstream stream(descriptor_path);
	rapidjson::IStreamWrapper stream_wrapper(stream);
	document.ParseStream(stream_wrapper);

	auto size = document.GetObject()["meta"].GetObject()["size"].GetObject();
	atlas->size.x = size["w"].GetInt();
	atlas->size.y = size["h"].GetInt();

	for (auto& entry : document.GetObject()["frames"].GetArray())
	{
		auto& frame = entry["frame"].GetObject();

		std::string filename = entry["filename"].GetString();
		auto texture_name = "block/" + filename.substr(0, filename.find('.'));

		atlas->texture_by_name.insert(std::make_pair(
			texture_name,
			Atlas::Texture{
				{frame["x"].GetInt(), frame["y"].GetInt()},
				{frame["w"].GetInt(), frame["h"].GetInt()}
			}
		));
	}

	return atlas;
}
