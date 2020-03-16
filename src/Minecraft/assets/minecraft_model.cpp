#include "minecraft_model.hpp"

#include <stdexcept>
#include <glm/gtx/transform.hpp>

#include "minecraft_assets.hpp"

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
	if (json.HasMember("uv"))
	{
		auto uv = json["uv"].GetArray();

		this->from_uv = glm::vec2(uv[0].GetFloat(), uv[1].GetFloat());
		this->to_uv = glm::vec2(uv[2].GetFloat(), uv[3].GetFloat());
	}
	else
	{
		// TODO: equal to the element face position.
		this->from_uv = glm::vec2(0);
		this->to_uv = glm::vec2(0);
	}

	this->texture = json["texture"].GetString();

	if (json.HasMember("rotation"))
		this->rotation = json["rotation"].GetInt();

	this->tint_index = json.HasMember("tintindex");
}

const Atlas::Texture& MinecraftModelElementFace::get_texture(const std::shared_ptr<const MinecraftAssets>& assets) const
{
	std::string variable = this->texture;
	const MinecraftModel* model = this->element->model;
	while (true)
	{
		if (assets->atlas->texture_by_name.find(variable) != assets->atlas->texture_by_name.end())
			return assets->atlas->texture_by_name.at(variable);

		if (model == nullptr) // There's no more parent to step in.
			throw std::runtime_error(
				std::string("Unresolved texture name: ") + variable
			);

		if (model->texture_variable_replacement.find(variable) != model->texture_variable_replacement.end())
			variable = model->texture_variable_replacement.at(variable);

		model = model->parent_model.empty() ? &model->get_parent_model(assets) : nullptr;
	}
}

size_t MinecraftModelElementFace::bake(std::vector<float>& buffer, const std::shared_ptr<const MinecraftAssets>& assets, glm::mat4 transform) const
{
	int axis = static_cast<int>(this->orientation) >> 1;
	int right = static_cast<int>(this->orientation) & 1;

	const int gray_sequence[]{0b00, 0b01, 0b11, 0b10};
	for (int gray_code : gray_sequence)
	{
		int mb = gray_code >> 1;
		int lb = gray_code & 1;

		// Position
		glm::vec3 position;

		int coord;
		coord = (axis + 0) % 3; position[coord] = right;
		coord = (axis + 1) % 3; position[coord] = lb;
		coord = (axis + 2) % 3; position[coord] = mb;

		position = transform * glm::vec4(position, 1);

		buffer.push_back(position.x);
		buffer.push_back(position.y);
		buffer.push_back(position.z);

		// UV
		auto texture = this->get_texture(assets);
		auto texture_from = (texture.from + this->from_uv) / assets->atlas->size;
		auto texture_to = (texture.from + this->to_uv) / assets->atlas->size;

		buffer.push_back(texture_from.x + lb * texture_to.x);
		buffer.push_back(texture_from.y + mb * texture_to.y);

		// Tint index
		buffer.push_back(this->tint_index);
	}

	return 4;
}

// ================================================================================================
// MinecraftModelElement
// ================================================================================================

using Orientation = MinecraftModelElementFace::Orientation;

const std::unordered_map<std::string, Orientation> orientation_by_name{
	{"down", Orientation::DOWN},
	{"up", Orientation::UP},
	{"north", Orientation::NORTH},
	{"south", Orientation::SOUTH},
	{"west", Orientation::WEST},
	{"east", Orientation::EAST}
};

MinecraftModelElement::MinecraftModelElement(const MinecraftModel* model) :
	model(model)
{}

void MinecraftModelElement::from_json(const rapidjson::Value::Object& json)
{
	auto from = json["from"].GetArray();
	vec3_from_json(this->from, from);

	auto to = json["to"].GetArray();
	vec3_from_json(this->to, to);

	if (json.HasMember("rotation"))
	{
		auto rotation = json["rotation"].GetObject();

		vec3_from_json(this->rotation.origin, rotation["origin"].GetArray());
		this->rotation.axis = rotation["axis"].GetString()[0];
		this->rotation.angle = rotation["angle"].GetFloat();
		this->rotation.rescale = rotation.HasMember("rescale") && rotation["rescale"].GetBool();
	}

	for (auto& member : json["faces"].GetObject())
	{
		MinecraftModelElementFace face(this);
		face.orientation = orientation_by_name.at(member.name.GetString());
		face.from_json(member.value.GetObject());
		faces_by_position.insert(std::make_pair(face.orientation, face));
	}
}

size_t MinecraftModelElement::bake(std::vector<float>& buffer, const std::shared_ptr<const MinecraftAssets>& assets, glm::mat4 transform) const
{
	size_t vertices_count = 0;
	for (auto& face : this->faces_by_position)
	{
		transform = glm::translate(transform, glm::vec3(this->from));
		transform = glm::scale(transform, glm::vec3(this->to - this->from));
		// TOOD: rotation

		vertices_count += face.second.bake(buffer, assets, transform);
	}
	return vertices_count;
}

// ================================================================================================
// MinecraftModel
// ================================================================================================

void MinecraftModel::from_json(const rapidjson::Value::Object& json)
{
	if (json.HasMember("parent"))
		this->parent_model = json["parent"].GetString();

	if (json.HasMember("textures"))
	{
		for (const auto& member : json["textures"].GetObject())
			this->texture_variable_replacement.insert(std::make_pair(member.name.GetString(), member.value.GetString()));
	}

	if (json.HasMember("elements"))
	{
		for (auto& object : json["elements"].GetArray())
		{
			MinecraftModelElement element(this);
			element.from_json(object.GetObject());
			this->elements.push_back(element);
		}
	}
}

MinecraftModel const& MinecraftModel::get_parent_model(std::shared_ptr<MinecraftAssets const> const& assets) const
{
	return assets->model_by_name.at(this->parent_model);
}

size_t MinecraftModel::bake(std::vector<float>& buffer, const std::shared_ptr<const MinecraftAssets>& pool, glm::mat4 transform) const
{
	size_t vertices_count = 0;

	if (!this->parent_model.empty())
		vertices_count += this->get_parent_model(pool).bake(buffer, pool, transform);

	for (auto& element : this->elements)
		vertices_count += element.bake(buffer, pool, transform);

	return vertices_count;
}
