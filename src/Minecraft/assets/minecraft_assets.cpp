#include "minecraft_assets.hpp"

#include <fstream>
#include <stdexcept>
#include <thread>
#include <string>
#include <iostream>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#include "util/atlas/texture_packer_atlas_loader.hpp"

#include <ThreadPool.h>

namespace fs = std::filesystem;

void load_json(std::filesystem::path const& path, std::function<void(rapidjson::Document&)> const& on_load)
{
	std::ifstream stream(path);
	rapidjson::IStreamWrapper stream_wrapper(stream);

	rapidjson::Document document;
	document.ParseStream(stream_wrapper);

	on_load(document);
}

void load_model(std::filesystem::path const& path, std::function<void(MinecraftModel const&)> const& on_load)
{
	load_json(path, [&](rapidjson::Document& document) {
		MinecraftModel model;
		model.from_json(document.GetObject());

		on_load(model);
	});
}

void load_block_state(std::filesystem::path const& path, std::function<void(MinecraftBlockState const&)> const& on_load)
{
	load_json(path, [&](rapidjson::Document& document) {
		MinecraftBlockState block_state;
		block_state.from_json(document.GetObject());
		on_load(block_state);
	});
}

MinecraftAssets::MinecraftAssets(std::string const& name) :
	name(name)
{}

std::shared_ptr<MinecraftAssets const> MinecraftAssets::load(std::filesystem::path const& path, std::string const& version_name, OnFileLoad const& on_file_load)
{
	auto assets = std::make_shared<MinecraftAssets>(version_name);
	auto version_folder = path / version_name;

	// Atlas
	auto atlas_folder = version_folder / "assets" / "minecraft" / "textures" / "block";
	auto atlas_image_path = atlas_folder / "atlas.png";
	auto atlas_descriptor_path = atlas_folder / "atlas.json";

	assets->atlas = TexturePackerAtlasLoader::load(atlas_image_path, atlas_descriptor_path);

	on_file_load(atlas_image_path);
	on_file_load(atlas_descriptor_path);

	// Model
	auto model_folder = version_folder / "assets" / "minecraft" / "models" / "block";
	for (auto const& file : std::filesystem::directory_iterator(model_folder))
	{
		load_model(file.path(), [&, file](MinecraftModel const& model) {
			auto name = std::string("block/") + file.path().stem().u8string();
			assets->model_by_name.insert(std::make_pair(name, model));
			on_file_load(file.path());
		});
	}

	// Block state
	auto block_state_folder = version_folder / "assets" / "minecraft" / "blockstates";
	for (auto const& file : std::filesystem::directory_iterator(block_state_folder))
	{
		load_block_state(file.path(), [&, file](MinecraftBlockState const& block_state) {
			auto name = file.path().stem().u8string();
			assets->block_state_by_name[name] = block_state;
			for (auto& variant : assets->block_state_by_name[name].variant_by_name)
			{
				auto variant_name = name + (variant.first.empty() ? "" : "[" + variant.first + "]");
				assets->block_state_variant_by_name[variant_name] = std::make_pair(variant_name, &variant.second);
				assets->block_state_variant_by_id.push_back(&assets->block_state_variant_by_name[variant_name]);
			}
			on_file_load(file.path());
		});
	}

	return assets;
}
