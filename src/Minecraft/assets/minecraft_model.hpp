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
	const MinecraftModelElement* element;

	enum class Orientation { WEST, EAST, DOWN, UP, SOUTH, NORTH };
	Orientation orientation;

	glm::vec2 from_uv, to_uv;
	std::string texture;
	//bool cull_face;
	unsigned int rotation;
	bool tint_index;

	MinecraftModelElementFace(const MinecraftModelElement* element);

	void from_json(const rapidjson::Value::Object& json);

	const Atlas::Texture& get_texture(const std::shared_ptr<const MinecraftAssets>& assets) const;

	size_t bake(std::vector<float>& buffer, const std::shared_ptr<const MinecraftAssets>& assets, glm::mat4 transform) const;
};

// ================================================================================================
// MinecraftModelElement
// ================================================================================================

struct MinecraftModelElement
{
	const MinecraftModel* model;

	glm::vec3 from, to;
	std::unordered_map<MinecraftModelElementFace::Orientation, MinecraftModelElementFace> faces_by_position;

	struct {
		glm::vec3 origin;
		char axis;
		float angle;
		bool rescale;
	} rotation;

	MinecraftModelElement(const MinecraftModel* model);

	void from_json(const rapidjson::Value::Object& json);

	size_t bake(std::vector<float>& buffer, const std::shared_ptr<const MinecraftAssets>& assets, glm::mat4 transform) const;
};

// ================================================================================================
// MinecraftModel
// ================================================================================================

struct MinecraftModel
{
	std::string parent_model;
	std::unordered_map<std::string, std::string> texture_variable_replacement;
	std::vector<MinecraftModelElement> elements;

	void from_json(const rapidjson::Value::Object& json);

	const MinecraftModel& get_parent_model(const std::shared_ptr<const MinecraftAssets>& assets) const;

	size_t bake(std::vector<float>& buffer, const std::shared_ptr<const MinecraftAssets>& assets, glm::mat4 transform) const;
};
