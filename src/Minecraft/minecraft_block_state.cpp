#include "minecraft_block_state.hpp"

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

const MinecraftModel& MinecraftBlockStateVariant::get_model(const std::shared_ptr<const MinecraftVersionPool>& pool) const
{
	return pool->get_model(this->model);
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
