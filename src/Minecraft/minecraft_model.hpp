#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include <glm/glm.hpp>

#include <rapidjson/document.h>

#include "minecraft_version_pool.hpp"

struct MinecraftModelElementFace;
struct MinecraftModelElement;
struct MinecraftModel;

// ================================================================================================
// MinecraftModelFace
// ================================================================================================

struct MinecraftModelElementFace
{
	const MinecraftModelElement* element;

	std::string position;
	glm::uvec2 from_uv, to_uv;
	std::string texture;
	//bool cull_face;
	unsigned int rotation;
	bool tint_index;

	MinecraftModelElementFace(const MinecraftModelElement* element);

	void from_json(const rapidjson::Value::Object& json);

	const Atlas::Texture& get_texture(const std::shared_ptr<const MinecraftVersionPool>& pool) const;

	void bake(std::vector<GLfloat>& buffer, const std::shared_ptr<const MinecraftVersionPool>& pool, const glm::mat4& transform) const;
};

// ================================================================================================
// MinecraftModelElement
// ================================================================================================

struct MinecraftModelElement
{
	const MinecraftModel* model;

	glm::ivec3 from, to;
	std::unordered_map<std::string, MinecraftModelElementFace> faces_by_position;

	struct {
		glm::vec3 origin;
		char axis;
		float angle;
		bool rescale;
	} rotation;

	MinecraftModelElement(const MinecraftModel* model);

	void from_json(const rapidjson::Value::Object& json);

	void bake(std::vector<GLfloat>& buffer, const std::shared_ptr<const MinecraftVersionPool>& pool, const glm::mat4& transform) const;
};

// ================================================================================================
// MinecraftModel
// ================================================================================================

struct MinecraftModel
{
	std::string parent_model;
	std::unordered_map<std::string, std::string> texture_variable_pool;
	std::vector<MinecraftModelElement> elements;

	void from_json(const rapidjson::Value::Object& json);

	const MinecraftModel& get_parent_model(const std::shared_ptr<const MinecraftVersionPool>& pool) const;

	void bake();
};
