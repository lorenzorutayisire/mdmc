#pragma once

#include <memory>
#include <filesystem>
#include <functional>

#include "util/atlas/atlas.hpp"

#include "minecraft_model.hpp"
#include "minecraft_block_state.hpp"

class MinecraftAssets
{
public:
	using OnFileLoad = std::function<void(std::filesystem::path const&)>;

	std::string name;

	std::shared_ptr<Atlas const> atlas;
	std::unordered_map<std::string, MinecraftModel> model_by_name;
	std::unordered_map<std::string, MinecraftBlockState> block_state_by_name;

	std::unordered_map<std::string, std::pair<std::string, MinecraftBlockStateVariant*>> block_state_variant_by_name;
	std::vector<std::pair<std::string, MinecraftBlockStateVariant*>*> block_state_variant_by_id;

	MinecraftAssets(std::string const& name);

	static std::shared_ptr<MinecraftAssets const> load(std::filesystem::path const& path, std::string const& version_name, OnFileLoad const& on_file_load);
};
