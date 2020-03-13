#pragma once

#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

#include <rapidjson/document.h>

#include "minecraft_version_pool.hpp"

// ================================================================================================
// MinecraftBlockStateVariant
// ================================================================================================

struct MinecraftBlockStateVariant
{
	std::string model;
	glm::uint x, y;
	bool uvlock;

	void from_json(const rapidjson::Value::Object& json);

	const MinecraftModel& get_model(const std::shared_ptr<const MinecraftVersionPool>& pool) const;
};

// ================================================================================================
// MinecraftBlockState
// ================================================================================================

struct MinecraftBlockState
{
	std::unordered_map<std::string, MinecraftBlockStateVariant> variant_by_name;

	void from_json(const rapidjson::Value::Object& json);
};
