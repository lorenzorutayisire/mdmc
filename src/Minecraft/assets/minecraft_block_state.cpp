#include "minecraft_block_state.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "minecraft_assets.hpp"

// ================================================================================================
// MinecraftBlockStateVariant
// ================================================================================================

void MinecraftBlockStateVariant::from_json(const rapidjson::Value::Object& json)
{
	this->model = json["model"].GetString();
	this->x = json.HasMember("x") ? json["x"].GetInt() : 0;
	this->y = json.HasMember("y") ? json["y"].GetInt() : 0;
	//this->uvlock = json["uvlock"].GetBool();
}

const MinecraftModel& MinecraftBlockStateVariant::get_model(const std::shared_ptr<const MinecraftAssets>& assets) const
{
	return assets->model_by_name.at(this->model);
}

// ================================================================================================
// MinecraftBlockState
// ================================================================================================

void MinecraftBlockState::from_json(const rapidjson::Value::Object& json)
{
	if (json.HasMember("variants"))
	{
		for (auto& member : json["variants"].GetObject())
		{
			MinecraftBlockStateVariant variant;
			variant.from_json(member.value.IsArray() ? member.value.GetArray()[0].GetObject() : member.value.GetObject());
			this->variant_by_name.insert(std::make_pair(member.name.GetString(), variant));
		}
	}
	// multiparts
}

size_t MinecraftBlockStateVariant::bake(std::vector<float>& buffer, const std::shared_ptr<const MinecraftAssets>& assets, glm::mat4 transform) const
{
	transform = glm::translate(transform, glm::vec3(8, 8, 8));

	transform = glm::rotate(transform, glm::radians<float>(this->x), glm::vec3(1, 0, 0));
	transform = glm::rotate(transform, glm::radians<float>(this->y), glm::vec3(0, 1, 0));

	transform = glm::translate(transform, -glm::vec3(8, 8, 8));

	return this->get_model(assets).bake(assets, transform, buffer);
}
