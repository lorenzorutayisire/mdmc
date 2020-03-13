#include "minecraft_version_pool.hpp"

#include <fstream>
#include <stdexcept>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#include "util/atlas/texture_packer_atlas_loader.hpp"

namespace fs = std::filesystem;

const MinecraftModel& MinecraftVersionPool::get_model(const std::string& name) const
{
	return this->model_by_name.at(name);
}

const std::pair<const std::string, const MinecraftBlockStateVariant>& MinecraftVersionPool::get_block_state_variant(glm::uint id) const
{
	return this->variant_by_id.at(id);
}

std::shared_ptr<const MinecraftVersionPool> MinecraftVersionPool::load(const fs::path& path)
{
	auto root_path = path / "assets" / "minecraft";

	std::shared_ptr<MinecraftVersionPool> result;

	// textures
	auto textures_path = root_path / "textures";

	auto atlas_path = textures_path / "atlas.png";
	auto atlas_descriptor_path = textures_path / "atlas.json";

	if (!fs::exists(atlas_path) || !fs::exists(atlas_descriptor_path))
		throw std::invalid_argument(
			"atlas.png or atlas.json not found in textures folder. You must generate them manually with TexturePackerGUI."
		);

	if (!fs::is_regular_file(atlas_path) || !fs::is_regular_file(atlas_descriptor_path))
		throw std::invalid_argument(
			"atlas.png or atlas.json exists but they are not regular files."
		);

	result->atlas = TexturePackerAtlasLoader::load(atlas_path, atlas_descriptor_path);

	// models
	auto models_path = root_path / "models" / "block";
	for (const auto& file : std::filesystem::directory_iterator(models_path))
	{
		std::ifstream stream(file);
		rapidjson::IStreamWrapper stream_wrapper(stream);

		rapidjson::Document document;
		document.ParseStream(stream_wrapper);

		MinecraftModel model;
		model.from_json(document.GetObject());

		auto name = std::string("block/") + file.path().stem().u8string();
		result->model_by_name.insert(std::make_pair(name, model));
	}

	// block_states
	auto block_states = root_path / "blockstates";
	for (const auto& file : std::filesystem::directory_iterator(block_states))
	{
		std::ifstream stream(file);
		rapidjson::IStreamWrapper stream_wrapper(stream);

		rapidjson::Document document;
		document.ParseStream(stream_wrapper);

		MinecraftBlockState block_state;
		block_state.from_json(document.GetObject());

		for (auto& variant : block_state.variant_by_name)
			result->variant_by_id.push_back(variant);
	}

	return result;
}