#include "Assets.hpp"

#include <filesystem>
#include <stb_image.h>

#include <fstream>

// Just for debug purposes.
#include <iostream>
#include <chrono>

#include <rapidjson/istreamwrapper.h>

#include <glm/gtc/matrix_transform.hpp>

#define TEXTURES_PATH(path)		  (path / "textures")
#define TEXTURES_BLOCK_PATH(path) (TEXTURES_PATH(path) / "block")

#define MODELS_PATH(path)		  (path / "models")
#define MODELS_BLOCK_PATH(path)   (MODELS_PATH(path) / "block")

#define BLOCK_STATES_PATH(path)   (path / "blockstates")

#define BIN_PATH(path)		            (path / "bin")
#define BIN_ATLAS_PATH(path)            (BIN_PATH(path) / "atlas.png")
#define BIN_ATLAS_DESCRIPTOR_PATH(path) (BIN_PATH(path) / "atlas.json")

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

Assets::Assets(const std::filesystem::path& base_path, const std::string& version) :
	base_path(base_path),
	version(version)
{
}

const std::filesystem::path Assets::path() const
{
	return this->base_path / version / "assets" / "minecraft";
}

const Block& Assets::get_block(size_t id) const
{
	return this->blocks_by_name[id];
}

const std::vector<Block>& Assets::get_blocks() const
{
	return this->blocks_by_name;
}

GLuint Assets::get_atlas() const
{
	return this->atlas;
}

GLuint Assets::get_vbo() const
{
	return this->vbo;
}

size_t Assets::get_vertices_count() const
{
	return this->vertices_count;
}

int Assets::load_model_face(
	const std::unordered_map<std::string, std::string>& textures_variables,
	const Atlas& atlas,

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
		auto texture_var_name = texture_name.substr(1);

		if (textures_variables.find(texture_var_name) == textures_variables.end())
			return -1; // Unable to solve the texture_variable name.

		texture_name = textures_variables.at(texture_var_name);
	}

	if (atlas.textures_by_name.find(texture_name) == atlas.textures_by_name.end())
		return -1;

	auto& texture = atlas.textures_by_name.at(texture_name);

	auto from = element["from"].GetArray();
	auto to = element["to"].GetArray();

	for (auto i = 0; i < 2; i++)
	{
		for (auto j = 0; j < 2; j++)
		{
			auto k = abs(i - j);

			glm::vec4 position;
			glm::vec2 uv;

			/* Position */
			if (face_orientation / 2 == 0)
			{
				position = glm::vec4(
					face_orientation % 2 == 1 ? to[0].GetFloat() : from[0].GetFloat(),
					k ? to[1].GetFloat() : from[1].GetFloat(),
					i ? to[2].GetFloat() : from[2].GetFloat(),
					1
				);

				uv.x = position.z;
				uv.y = position.y;
			}
			else if (face_orientation / 2 == 1)
			{
				position = glm::vec4(
					k ? to[0].GetFloat() : from[0].GetFloat(),
					face_orientation % 2 == 1 ? to[1].GetFloat() : from[1].GetFloat(),
					i ? to[2].GetFloat() : from[2].GetFloat(),
					1
				);

				uv.x = position.x;
				uv.y = position.z;
			}
			else if (face_orientation / 2 == 2)
			{
				position = glm::vec4(
					k ? to[0].GetFloat() : from[0].GetFloat(),
					i ? to[1].GetFloat() : from[1].GetFloat(),
					face_orientation % 2 == 1 ? to[2].GetFloat() : from[2].GetFloat(),
					1
				);

				uv.x = position.x;
				uv.y = position.y;
			}

			position = transformation * position;

			vertices.push_back(position.x);
			vertices.push_back(position.y);
			vertices.push_back(position.z);

			/* UV */
			if (face.HasMember("uv"))
			{
				uv = glm::vec2(
					16 - face["uv"].GetArray()[i * 2].GetFloat(),
					16 - face["uv"].GetArray()[k * 2 + 1].GetFloat()
				);
			}

			// Atlas-absolute UV
			uv.x += texture.x;
			uv.y += texture.y;

			// Normalize UV (previously were Minecraft pixels 0->16)
			uv.x /= (float)atlas.w;
			uv.y /= (float)atlas.h;

			vertices.push_back(uv.x);
			vertices.push_back(uv.y);
		}
	}

	return 4;
}

auto orientation_by_name = std::unordered_map<std::string, ModelFaceOrientation>{
	{"west", WEST},
	{"east", EAST},
	{"down", DOWN},
	{"up", UP},
	{"south", SOUTH},
	{"north", NORTH},
};

int Assets::load_model_element(const std::unordered_map<std::string, std::string>& textures_variables, const Atlas& atlas, const ModelElement& element, glm::mat4 transformation, std::vector<GLfloat>& vertices)
{
	if (element.HasMember("rotation"))
	{
		auto& rotation = element["rotation"].GetObject();

		glm::vec3 pivot(
			rotation["origin"].GetArray()[0].GetFloat(),
			rotation["origin"].GetArray()[1].GetFloat(),
			rotation["origin"].GetArray()[2].GetFloat()
		);

		transformation = glm::translate(
			transformation,
			-pivot
		);

		glm::vec3 axis;
		if (rotation["axis"].GetString() == "x")
			axis = glm::vec3(1, 0, 0);
		else if (rotation["axis"].GetString() == "y")
			axis = glm::vec3(0, 1, 0);
		else if (rotation["axis"].GetString() == "z")
			axis = glm::vec3(0, 0, 1);
		else
			return false;

		transformation = glm::rotate(
			transformation,
			glm::radians(rotation["angle"].GetFloat()),
			axis
		);

		transformation = glm::translate(
			transformation,
			pivot
		);

		// rescale?
	}

	uint32_t vertices_count = 0;

	for (auto& member : element["faces"].GetObject())
	{
		vertices_count += this->load_model_face(
			textures_variables,
			atlas,
			orientation_by_name[member.name.GetString()],
			member.value.GetObject(),
			element,
			transformation,
			vertices
		);

		if (vertices_count < 0)
			return -1;
	}

	return vertices_count;
}

int Assets::load_model(
	const std::string& id,

	std::unordered_map<std::string, std::string>& textures_variables,
	const Atlas& atlas,

	glm::mat4 transformation,
	std::vector<GLfloat>& vertices,

	bool can_store
)
{
	auto model = read_json(MODELS_PATH(this->path()) / (id + ".json"))->GetObject();

	if (model.HasMember("textures"))
	{
		for (auto& member : model["textures"].GetObject())
		{
			if (member.name.GetString() != "particle") // We don't care about particle effect.
				textures_variables.insert(std::make_pair(member.name.GetString(), member.value.GetString()));
		}
	}

	uint32_t vertices_count = 0;

	if (can_store && model.HasMember("elements"))
	{
		for (auto& element : model["elements"].GetArray())
		{
			vertices_count += this->load_model_element(
				textures_variables,
				atlas,
				element.GetObject(),
				transformation,
				vertices
			);

			if (vertices_count < 0)
				return -1;
		}
		can_store = false; // If the model itself stores elements, the parent can't.
	}

	if (model.HasMember("parent"))
	{
		return this->load_model(
			model["parent"].GetString(),
			textures_variables,
			atlas,
			transformation,
			vertices,
			can_store
		);
	}

	return vertices_count;
}

void Assets::retrieve()
{
	// Nope! Currently the Minecraft's assets should already lie at "tmp/mc_assets".
}

Atlas Assets::load_atlas_descriptor()
{
	Atlas atlas;

	rapidjson::Document document;

	std::ifstream stream(BIN_ATLAS_DESCRIPTOR_PATH(this->path()));
	rapidjson::IStreamWrapper stream_wrapper(stream);

	document.ParseStream(stream_wrapper);

	/* meta */
	auto size = document.GetObject()["meta"].GetObject()["size"].GetObject();
	atlas.w = size["w"].GetInt();
	atlas.h = size["h"].GetInt();

	/* frames */
	for (auto& entry : document.GetObject()["frames"].GetArray())
	{
		auto& frame = entry["frame"].GetObject();

		std::string filename = entry["filename"].GetString();
		auto texture_name = "block/" + filename.substr(0, filename.find('.'));

		atlas.textures_by_name.insert(std::make_pair(
			texture_name,
			Texture{
				(uint32_t)frame["x"].GetInt(),
				(uint32_t)frame["y"].GetInt(),
				(uint32_t)frame["w"].GetInt(),
				(uint32_t)frame["h"].GetInt()
			}
		));
	}

	return atlas;
}

void Assets::load_atlas()
{
	glGenTextures(1, &this->atlas);
	glBindTexture(GL_TEXTURE_2D, this->atlas);

	int width, height;
	stbi_uc* image_data = stbi_load(
		BIN_ATLAS_PATH(this->path()).u8string().c_str(),
		&width,
		&height,
		nullptr,
		STBI_rgb_alpha
	);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	stbi_image_free(image_data);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Assets::load_block_states(const Atlas& atlas, bool forced)
{
	std::vector<GLfloat> vertices;
	this->vertices_count = 0;

	uint32_t blocks_count = 0;

	for (auto& entry : std::filesystem::directory_iterator(BLOCK_STATES_PATH(this->path())))
	{
		auto path = entry.path();

		auto blockstate = read_json(path)->GetObject();
		auto blockstate_name = path.stem().u8string();

		if (!blockstate.HasMember("variants"))
		{
			//std::cerr << "Block without variants: " << blockstate_name << std::endl;
			continue;
		}

		for (auto& variant_member : blockstate["variants"].GetObject())
		{
			auto variant = variant_member.value.IsArray() ? variant_member.value.GetArray()[0].GetObject() : variant_member.value.GetObject();

			glm::mat4 transformation(1.0f);
			transformation = glm::translate(transformation, glm::vec3(0.0f, 0.0f, 16 * blocks_count));

			// Translates the block to its own center in order to rotate around it.
			transformation = glm::translate(
				transformation,
				glm::vec3(8, 8, 8)
			);

			if (variant.HasMember("x"))
			{
				transformation = glm::rotate(
					transformation,
					glm::radians(variant["x"].GetFloat()),
					glm::vec3(1, 0, 0)
				);
			}

			if (variant.HasMember("y"))
			{
				transformation = glm::rotate(
					transformation,
					glm::radians(variant["y"].GetFloat()),
					glm::vec3(0, 1, 0)
				);
			}

			// Translates back the model to (0, 0, 0).
			transformation = glm::translate(
				transformation,
				glm::vec3(-8, -8, -8)
			);

			std::vector<GLfloat> model_vertices;

			auto model_name = variant["model"].GetString();

			int model_vertices_count = this->load_model(
				model_name,
				std::unordered_map<std::string, std::string>(),
				atlas,
				transformation,
				model_vertices
			);

			if (model_vertices_count < 0)
			{
				//std::cerr << "Block failed (probably due to an undefined texture): " << model_name << std::endl;
				continue;
			}

			blocks_count++;

			std::string variant_attr = variant_member.name.GetString();

			auto block_name = blockstate_name + (variant_attr.empty() ? "" : "[" + variant_attr + "]");
			this->blocks_by_name.push_back(Block{ block_name, vertices_count, (uint32_t)model_vertices_count });

			std::copy(model_vertices.begin(), model_vertices.end(), std::back_inserter(vertices));
			this->vertices_count += model_vertices_count;
		}
	}

	glGenBuffers(1, &this->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);
}

void Assets::load()
{
	this->load_atlas();
	Atlas atlas = this->load_atlas_descriptor();

	this->load_block_states(atlas);

	json_cache.clear(); // We don't need this dirty cache anymore. TODO clean me.
}

void Assets::render()
{
	/* Texture */
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->atlas);

	/* Color */
	glUniform4f(3, 1, 1, 1, 1);

	/* VBO */
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

	/* Position */
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

	/* UV */
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	glDrawArrays(GL_QUADS, 0, this->vertices_count);
}
