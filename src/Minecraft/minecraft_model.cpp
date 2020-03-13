#include "minecraft_model.hpp"

#include <stdexcept>

void ivec3_from_json(glm::ivec3 ivec3, const rapidjson::Value::Array& json)
{
	for (size_t i = 0; i < 3; i++)
		ivec3[i] = json[i].GetInt();
}

void vec3_from_json(glm::vec3 vec3, const rapidjson::Value::Array& json)
{
	for (size_t i = 0; i < 3; i++)
		vec3[i] = json[i].GetFloat();
}

// ================================================================================================
// MinecraftModelElementFace
// ================================================================================================

MinecraftModelElementFace::MinecraftModelElementFace(const MinecraftModelElement* element) :
	element(element)
{}

void MinecraftModelElementFace::from_json(const rapidjson::Value::Object& json)
{
	auto uv = json["uv"].GetArray();

	this->from_uv.x = uv[0].GetInt();
	this->from_uv.y = uv[1].GetInt();

	this->from_uv.x = uv[2].GetInt();
	this->from_uv.y = uv[3].GetInt();

	this->texture = json["texture"].GetString();
	// cullface
	this->rotation = json["rotation"].GetInt();

	this->tint_index = json.HasMember("tintindex");
}

const Atlas::Texture& MinecraftModelElementFace::get_texture(const std::shared_ptr<const MinecraftVersionPool>& pool) const
{
	std::string variable = this->texture;
	const MinecraftModel* model = this->element->model;
	while (true)
	{
		if (pool->atlas->texture_by_name.find(variable) != pool->atlas->texture_by_name.end())
			return pool->atlas->texture_by_name.at(variable);

		if (model == nullptr) // There's no more parent to step in.
			throw std::runtime_error(
				std::string("Unresolved texture name: ") + variable
			);

		if (model->texture_variable_pool.find(variable) != model->texture_variable_pool.end())
			variable = model->texture_variable_pool.at(variable);

		model = model->parent_model.empty() ? &model->get_parent_model(pool) : nullptr;
	}
}

void MinecraftModelElementFace::bake(std::vector<GLfloat>& buffer, const std::shared_ptr<const MinecraftVersionPool>& pool, const glm::mat4& transform) const
{
	if (this->position == "down")
	{
		buffer.push_back();
		buffer.push_back();
		buffer.push_back();
		buffer.push_back();
	}
	else if (this->position == "up")
	{

	}
	else if (this->position == "north")
	{

	}
	else if (this->position == "south")
	{

	}
	else if (this->position == "west")
	{

	}
	else if (this->position == "east")
	{

	}
	else
		throw std::invalid_argument(std::string("Face position not recognized: ") + this->position);
}

// ================================================================================================
// MinecraftModelElement
// ================================================================================================

MinecraftModelElement::MinecraftModelElement(const MinecraftModel* model) :
	model(model)
{}

void MinecraftModelElement::from_json(const rapidjson::Value::Object& json)
{
	auto from = json["from"].GetArray();
	ivec3_from_json(this->from, from);

	auto to = json["to"].GetArray();
	ivec3_from_json(this->to, to);

	auto rotation = json["rotation"].GetObject();

	vec3_from_json(this->rotation.origin, rotation["origin"].GetArray());
	this->rotation.axis = rotation["axis"].GetString()[0];
	this->rotation.angle = rotation["angle"].GetFloat();
	this->rotation.rescale = rotation["rescale"].GetBool();

	for (auto& member : json["faces"].GetObject())
	{
		MinecraftModelElementFace face(this);
		face.position = member.name.GetString();
		face.from_json(member.value.GetObject());
		faces_by_position.insert(std::make_pair(face.position, face));
	}
}

void MinecraftModelElement::bake(std::vector<GLfloat>& buffer, const std::shared_ptr<const MinecraftVersionPool>& pool, const glm::mat4& transform) const
{

}

// ================================================================================================
// MinecraftModel
// ================================================================================================

void MinecraftModel::from_json(const rapidjson::Value::Object& json)
{
	for (const auto& member : json["textures"].GetObject())
		this->texture_variable_pool.insert(std::make_pair(member.name.GetString(), member.value.GetString()));

	for (auto& object : json["elements"].GetArray())
	{
		MinecraftModelElement element(this);
		element.from_json(object.GetObject());
		this->elements.push_back(element);
	}
}

const MinecraftModel& MinecraftModel::get_parent_model(const std::shared_ptr<const MinecraftVersionPool>& pool) const
{
	return pool->get_model(this->parent_model);
}

