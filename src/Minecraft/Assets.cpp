#include "Assets.hpp"

#include <filesystem>
#include <stb_image.h>

#include <fstream>

// Just for debug purposes.
#include <iostream>
#include <chrono>

#include <rapidjson/istreamwrapper.h>

#include <glm/gtc/matrix_transform.hpp>

using namespace mdmc::Minecraft;

std::unordered_map<std::string, std::shared_ptr<rapidjson::Document>> json_cache;

std::shared_ptr<rapidjson::Document> read_json(const std::filesystem::path& path)
{
	std::string path_str = path.u8string();
	if (json_cache.find(path_str) == json_cache.end())
	{
		auto document = std::make_shared<rapidjson::Document>();

		std::ifstream stream(path);
		rapidjson::IStreamWrapper stream_wrapper(stream);

		document->ParseStream(stream_wrapper);

		json_cache.insert(std::make_pair(path_str, document));
	}
	return json_cache[path_str];
}

// =====================================================================================
// Assets
// =====================================================================================

void Assets::store_textures(const std::filesystem::path& path)
{
	size_t textures_count = 0;

	for (auto& entry : std::filesystem::directory_iterator(path / "block"))
		textures_count++;

	glGenTextures(1, &this->textures);

	glBindTexture(GL_TEXTURE_2D_ARRAY, this->textures);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 16, 16, textures_count);

	for (auto& entry : std::filesystem::directory_iterator(path / "block"))
	{
		auto texture_path = entry.path();

		if (texture_path.extension() == ".mcmeta")
			continue;

		auto texture_name = "block/" + texture_path.stem().u8string();

		int width, height;
		uint8_t* image_data = stbi_load(texture_path.u8string().c_str(), &width, &height, NULL, STBI_rgb_alpha);

		if (image_data == nullptr)
		{
			std::cerr << stbi_failure_reason << ": " << texture_name << std::endl;
			continue;
		}

		if (width > 16 || height > 16)
		{
			std::cerr << "Texture isn't 16x16: " << texture_name << " (" << width << "x" << height << ")" << std::endl;
			continue;
		}

		textures_count--;

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, textures_count, 16, 16, 1, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		this->textures_by_name.insert(std::make_pair(texture_name, textures_count));

		stbi_image_free(image_data);
	}

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

bool Assets::store_model_face(
	const std::unordered_map<std::string, std::string>& variables,

	ModelFaceOrientation face_orientation,
	const ModelFace& face,
	const ModelElement& element,

	glm::mat4 transformation,
	std::vector<GLfloat>& vertices
)
{
	std::string texture_name;

	texture_name = face["texture"].GetString();
	while (texture_name[0] == '#')
	{
		auto variable_name = texture_name.substr(1);

		if (variables.find(variable_name) == variables.end())
			return false; // Unable to solve the variable name.

		texture_name = variables.at(variable_name);
	}

	if (this->textures_by_name.find(texture_name) == this->textures_by_name.end())
		return false;

	uint32_t texture_id = this->textures_by_name[texture_name];

	auto from = element["from"].GetArray();
	auto to = element["to"].GetArray();

	for (auto i = 0; i < 2; i++)
	{
		for (auto j = 0; j < 2; j++)
		{
			glm::vec4 position;
			glm::vec2 uv;

			/* Position */
			if (face_orientation / 2 == 0)
			{
				position = glm::vec4(
					face_orientation % 2 == 1 ? to[0].GetFloat() : from[0].GetFloat(),
					i ? to[1].GetFloat() : from[1].GetFloat(),
					j ? to[2].GetFloat() : from[2].GetFloat(),
					1
				);

				if (!face.HasMember("uv"))
				{
					uv.x = position.y;
					uv.y = position.z;
				}
			}
			else if (face_orientation / 2 == 1)
			{
				position = glm::vec4(
					i ? to[0].GetFloat() : from[0].GetFloat(),
					face_orientation % 2 == 1 ? to[1].GetFloat() : from[1].GetFloat(),
					j ? to[2].GetFloat() : from[2].GetFloat(),
					1
				);

				if (!face.HasMember("uv"))
				{
					uv.x = position.x / 16.f;
					uv.y = position.z / 16.f;
				}
			}
			else if (face_orientation / 2 == 2)
			{
				position = glm::vec4(
					i ? to[0].GetFloat() : from[0].GetFloat(),
					j ? to[1].GetFloat() : from[1].GetFloat(),
					face_orientation % 2 == 1 ? to[2].GetFloat() : from[2].GetFloat(),
					1
				);

				if (!face.HasMember("uv"))
				{
					uv.x = position.x;
					uv.y = position.y;
				}
			}

			position = transformation * position;

			vertices.push_back(position.x);
			vertices.push_back(position.y);
			vertices.push_back(position.z);

			/* UV */
			if (face.HasMember("uv"))
			{
				uv = glm::vec2(
					face["uv"].GetArray()[i * 2].GetFloat() / 16.f,
					face["uv"].GetArray()[j * 2 + 1].GetFloat() / 16.f
				);
			}
			
			vertices.push_back(uv.x);
			vertices.push_back(uv.y);

			/* Texture */
			vertices.push_back((GLfloat)texture_id);

			/* Tint-index */
			if (!face.HasMember("tintindex"))
			{
				vertices.push_back(1.0f);
				vertices.push_back(1.0f);
				vertices.push_back(1.0f);
				vertices.push_back(1.0f);
			}
			else
			{
				// If tint-index is set, by default sets a leaf-like color.
				vertices.push_back(87.f / 255.f);
				vertices.push_back(179.f / 255.f);
				vertices.push_back(59.f / 255.f);
				vertices.push_back(1.0f);
			}
		}
	}

	return true;
}

auto orientation_by_name = std::unordered_map<std::string, ModelFaceOrientation>{
	{"west", WEST},
	{"east", EAST},
	{"down", DOWN},
	{"up", UP},
	{"south", SOUTH},
	{"north", NORTH},
};

bool Assets::store_model_element(
	const std::unordered_map<std::string, std::string>& variables,

	const ModelElement& element,

	glm::mat4 transformation,
	std::vector<GLfloat>& vertices
)
{
	for (auto& member : element["faces"].GetObject())
	{
		if (!this->store_model_face(
			variables,
			orientation_by_name[member.name.GetString()],
			member.value.GetObject(),
			element,
			transformation,
			vertices
		)) return false;
	}

	return true;
}

bool Assets::store_model(
	const std::filesystem::path& base_path,
	const std::string& id,
	std::unordered_map<std::string, std::string>& variables,

	glm::mat4 transformation,
	std::vector<GLfloat>& vertices,

	bool can_store
)
{
	auto model = read_json(base_path / (id + ".json"))->GetObject();

	if (model.HasMember("textures"))
	{
		for (auto& member : model["textures"].GetObject())
		{
			if (member.name.GetString() != "particle") // We don't care about particle effect.
				variables.insert(std::make_pair(member.name.GetString(), member.value.GetString()));
		}
	}

	if (can_store && model.HasMember("elements"))
	{
		for (auto& element : model["elements"].GetArray())
		{
			if (!this->store_model_element(variables, element.GetObject(), transformation, vertices))
				return false;
		}
		can_store = false; // If the model itself stores elements, the parent can't.
	}

	if (model.HasMember("parent"))
	{
		return this->store_model(
			base_path,
			model["parent"].GetString(),
			variables,
			transformation,
			vertices,
			can_store
		);
	}

	return true;
}

void Assets::store_blocks(const std::filesystem::path& base_path)
{
	std::vector<GLfloat> vertices;
	glm::mat4 transformation(1.0f);

	for (auto& entry : std::filesystem::directory_iterator(base_path / "blockstates"))
	{
		auto path = entry.path();

		auto blockstate = read_json(path)->GetObject();
		auto blockstate_name = path.stem().u8string();

		if (!blockstate.HasMember("variants"))
		{
			std::cerr << "Block without variants: " << blockstate_name << std::endl;
			continue;
		}

		for (auto& variant_member : blockstate["variants"].GetObject())
		{
			auto variant = variant_member.value.IsArray() ? variant_member.value.GetArray()[0].GetObject() : variant_member.value.GetObject();
			auto block_name = blockstate_name + "[" + variant_member.name.GetString() + "]";

			auto model_name = variant["model"].GetString();

			std::vector<GLfloat> model;
			auto model_loaded = this->store_model(
				base_path / "models",
				model_name,
				std::unordered_map<std::string, std::string>(),
				transformation,
				model
			);

			// Translates the model of a block every time, to get a row.
			//transformation = glm::translate(transformation, glm::vec3(0.0f, 0.0f, 1.0f));

			if (!model_loaded)
			{
				std::cerr << "Block failed (probably due to an undefined texture): " << model_name << std::endl;
				continue;
			}

			blocks_by_id.push_back(Block{ block_name, vertices.size(), model.size() });
			std::copy(model.begin(), model.end(), std::back_inserter(vertices)); // If the model loaded successfully, copies the model inside of the VBO.
		}
	}

	glGenBuffers(1, &this->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);

	this->vertices_count = vertices.size();
}

void Assets::store(const std::filesystem::path& base_path)
{
	Assets* assets = new Assets();

	auto assets_path = base_path / "assets" / "minecraft";

	this->store_textures(assets_path / "textures");
	this->store_blocks(assets_path);

	std::cout << "Loaded " << this->textures_by_name.size() << " textures." << std::endl;
	std::cout << "Loaded " << this->blocks_by_id.size() << " blocks." << std::endl;

	json_cache.clear(); // We don't need this dirty cache anymore. TODO clean me.
}
