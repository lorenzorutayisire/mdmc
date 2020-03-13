#pragma once

#include <memory>
#include <filesystem>

#include "util/atlas/atlas.hpp"

#include "minecraft_model.hpp"
#include "minecraft_block_state.hpp"

class MinecraftVersionPool
{
private:
	std::unordered_map<std::string, MinecraftModel> model_by_name;
	std::vector<std::pair<std::string, MinecraftBlockStateVariant>> variant_by_id;

public:
	std::shared_ptr<const Atlas> atlas;
	const MinecraftModel& get_model(const std::string& name) const;
	const std::pair<const std::string, const MinecraftBlockStateVariant>& get_block_state_variant(glm::uint id) const;

	static std::shared_ptr<const MinecraftVersionPool> load(const std::filesystem::path& path);
};
