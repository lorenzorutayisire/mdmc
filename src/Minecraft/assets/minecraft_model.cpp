#include "minecraft_model.hpp"

#include <stdexcept>
#include <string>
#include <iostream>

#include <glm/gtx/transform.hpp>

#include "minecraft_assets.hpp"

void ivec3_from_json(glm::ivec3& ivec3, const rapidjson::Value::Array& json)
{
	for (size_t i = 0; i < 3; i++)
		ivec3[i] = json[i].GetInt();
}

void vec3_from_json(glm::vec3& vec3, const rapidjson::Value::Array& json)
{
	for (size_t i = 0; i < 3; i++)
		vec3[i] = json[i].GetFloat();
}

// ================================================================================================
// MinecraftModelElementFace
// ================================================================================================

void MinecraftModelElementFace::from_json(
	MinecraftModelElement const* element,
	rapidjson::Value::Object const& json
)
{
	if (json.HasMember("uv"))
	{
		auto uv = json["uv"].GetArray();

		this->from_uv = glm::vec2(uv[0].GetFloat(), uv[1].GetFloat());
		this->to_uv = glm::vec2(uv[2].GetFloat(), uv[3].GetFloat());
	}
	else
	{
		this->from_uv = element->from;
		this->to_uv = element->to;
	}

	this->texture = json["texture"].GetString();

	this->rotation = json.HasMember("rotation") ? json["rotation"].GetInt() : 0;

	this->tint_index = json.HasMember("tintindex");
}

const Atlas::Texture& MinecraftModelElementFace::get_texture(
	std::shared_ptr<MinecraftAssets const> const& assets,
	std::unordered_map<std::string, std::string> const& texture_by_variable
) const
{
	auto atlas = assets->atlas;
	std::string texture = this->texture;
	while (texture[0] == '#')
	{
		auto variable = texture.substr(1);
		if (texture_by_variable.find(variable) != texture_by_variable.end())
			texture = texture_by_variable.at(variable);
		else
			throw std::runtime_error(std::string("Unresolved texture variable: ") + texture);
	}
	return atlas->texture_by_name.at(texture);
}

size_t MinecraftModelElementFace::bake(
	std::shared_ptr<MinecraftAssets const> const& assets,
	std::unordered_map<std::string, std::string> const& texture_by_variable,
	glm::mat4 transform,
	std::vector<float>& buffer
) const
{
	int axis = static_cast<int>(this->orientation) >> 1;
	int right = static_cast<int>(this->orientation) & 1;

	const int gray_sequence[]{0b00, 0b01, 0b11, 0b10};
	for (int i = 0; i < 4; i++)
	{
		int gray_code = gray_sequence[i];
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
		auto texture = this->get_texture(assets, texture_by_variable);
		auto texture_from = texture.from + this->from_uv;
		auto texture_size = this->to_uv - this->from_uv;
		
		auto& rot_gray_code = gray_sequence[(i + (this->rotation / 90) % 4) % 4];
		int rot_mb = rot_gray_code >> 1;
		int rot_lb = rot_gray_code & 1;

		buffer.push_back(texture_from.x + rot_lb * texture_size.x);
		buffer.push_back(texture_from.y + rot_mb * texture_size.y);

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
	else
		this->rotation.angle = 0;

	for (auto& member : json["faces"].GetObject())
	{
		MinecraftModelElementFace face;
		face.orientation = orientation_by_name.at(member.name.GetString());
		face.from_json(this, member.value.GetObject());
		this->face_by_position.insert(std::make_pair(face.orientation, face));
	}

	// If the element adds faces but not with all orientations, here we add them.
	// This is done to ensure baking consistency.
	if (this->face_by_position.size() > 0 && this->face_by_position.size() < 6)
	{
		for (auto& member : orientation_by_name)
		{
			if (this->face_by_position.find(member.second) == this->face_by_position.end())
			{
				MinecraftModelElementFace face;
				face.orientation = member.second;
				face.from_uv = glm::vec3(0);
				face.to_uv = glm::vec3(0);
				face.texture = "block/torch"; // Some dummy texture invisible at (0, 0).
				face.rotation = 0;
				face.tint_index = false;
				this->face_by_position.insert(std::make_pair(face.orientation, face));
			}
		}
	}
}

size_t MinecraftModelElement::bake(
	std::shared_ptr<MinecraftAssets const> const& assets,
	std::unordered_map<std::string, std::string> const& texture_by_variable,
	glm::mat4 transform,
	std::vector<float>& buffer
) const
{
	if (this->rotation.angle != 0)
	{
		transform = glm::translate(transform, this->rotation.origin);
		transform = glm::rotate(
			transform,
			glm::radians(this->rotation.angle),
			glm::vec3(
				this->rotation.axis == 'x' ? 1 : 0,
				this->rotation.axis == 'y' ? 1 : 0,
				this->rotation.axis == 'z' ? 1 : 0
			)
		);
		transform = glm::translate(transform, -this->rotation.origin);
	}

	transform = glm::translate(transform, glm::vec3(this->from));
	transform = glm::scale(transform, glm::vec3(this->to - this->from));

	size_t vertices_count = 0;

	// Some models may only be implementations and their faces are defined in parent models.
	if (this->face_by_position.size() > 0)
	{
		for (int i = 0; i < static_cast<int>(MinecraftModelElementFace::Orientation::size); i++)
		{
			auto orientation = static_cast<MinecraftModelElementFace::Orientation>(i);
			auto face = this->face_by_position.at(orientation);
			vertices_count += face.bake(assets, texture_by_variable, transform, buffer);
		}
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
			this->texture_by_variable.insert(std::make_pair(member.name.GetString(), member.value.GetString()));
	}

	if (json.HasMember("elements"))
	{
		for (auto& object : json["elements"].GetArray())
		{
			MinecraftModelElement element;
			element.from_json(object.GetObject());
			this->elements.push_back(element);
		}
	}
}

MinecraftModel const& MinecraftModel::get_parent_model(std::shared_ptr<MinecraftAssets const> const& assets) const
{
	return assets->model_by_name.at(this->parent_model);
}

size_t MinecraftModel::bake(
	std::shared_ptr<MinecraftAssets const> const& assets,
	glm::mat4 transform,
	std::vector<float>& buffer
) const
{
	size_t vertices_count = 0;

	MinecraftModel const* model = this;
	std::unordered_map<std::string, std::string> texture_by_variable;

	while (model)
	{
		texture_by_variable.insert(model->texture_by_variable.begin(), model->texture_by_variable.end());
		if (model->elements.size() > 0)
		{
			for (auto& element : model->elements)
				vertices_count += element.bake(assets, texture_by_variable, transform, buffer);
			return vertices_count;
		}
		model = !model->parent_model.empty() ? &model->get_parent_model(assets) : nullptr;
	}

	return 0;
}
