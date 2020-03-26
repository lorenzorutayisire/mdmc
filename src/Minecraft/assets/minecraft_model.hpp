#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include <glm/glm.hpp>
#include <rapidjson/document.h>

#include "util/atlas/atlas.hpp"

class MinecraftAssets;

struct MinecraftModelElementFace;
struct MinecraftModelElement;
struct MinecraftModel;

// ================================================================================================
// MinecraftModelFace
// ================================================================================================

struct MinecraftModelElementFace
{
	enum class Orientation
	{
		WEST,
		EAST,
		DOWN,
		UP,
		SOUTH,
		NORTH,
		size
	};

	Orientation orientation;

	glm::vec2 from_uv, to_uv;
	std::string texture;
	//bool cull_face;
	unsigned int rotation;
	bool tint_index;

	void from_json(
		MinecraftModelElement const* element,
		const rapidjson::Value::Object& json
	);

	const Atlas::Texture& get_texture(std::shared_ptr<MinecraftAssets const> const& assets, std::unordered_map<std::string, std::string> const& texture_by_variable) const;

	size_t bake(
		std::shared_ptr<MinecraftAssets const> const& assets,
		std::unordered_map<std::string, std::string> const& texture_by_variable,
		glm::mat4 transform,
		std::vector<float>& buffer
	) const;
};

// ================================================================================================
// MinecraftModelElement
// ================================================================================================

struct MinecraftModelElement
{
	glm::vec3 from, to;
	std::unordered_map<MinecraftModelElementFace::Orientation, MinecraftModelElementFace> faces_by_position;

	struct {
		glm::vec3 origin;
		char axis;
		float angle;
		bool rescale;
	} rotation;

	void from_json(const rapidjson::Value::Object& json);

	size_t bake(std::shared_ptr<MinecraftAssets const> const& assets, std::unordered_map<std::string, std::string> const& texture_by_variable, glm::mat4 transform, std::vector<float>& buffer) const;
};

// ================================================================================================
// MinecraftModel
// ================================================================================================

struct MinecraftModel
{
	std::string parent_model;
	std::unordered_map<std::string, std::string> texture_by_variable;
	std::vector<MinecraftModelElement> elements;

	void from_json(const rapidjson::Value::Object& json);

	const MinecraftModel& get_parent_model(const std::shared_ptr<const MinecraftAssets>& assets) const;

	size_t bake(std::shared_ptr<MinecraftAssets const> const& assets, glm::mat4 transform, std::vector<float>& buffer) const;
};
