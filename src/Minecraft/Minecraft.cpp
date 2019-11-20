#include "Minecraft.hpp"

#include <filesystem>
#include <stb_image.h>

#include <fstream>

// Just for debug purposes.
#include <iostream>
#include <chrono>

#include <rapidjson/istreamwrapper.h>

using namespace Minecraft;

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

/****************************************************************** textures */

void load_textures(const std::filesystem::path& path, Assets* assets)
{
	auto t0 = std::chrono::high_resolution_clock::now();

	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(path / "block"))
	{
		const std::string file_path = entry.path().u8string().c_str();
		int width, height;
		uint8_t* data = stbi_load(file_path.c_str(), &width, &height, NULL, STBI_rgb_alpha);

		const std::string id = "block/" + entry.path().stem().u8string();
		assets->textures.insert(std::make_pair(id, new Texture{ data }));
	}

	auto t1 = std::chrono::high_resolution_clock::now();
	std::cout << "Loaded " << assets->textures.size() << " textures in: " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << "ms" << std::endl;
}

/****************************************************************** models */

/* Model_Face */

bool load_model_face(Model_Element& element, std::string position, rapidjson::Value::Object& json, std::unordered_map<std::string, std::string>& variables, Assets* assets)
{
	std::string texture_id;

	texture_id = json["texture"].GetString();
	while (texture_id[0] == '#')
	{
		std::string variable_name = texture_id.substr(1);
		if (variables.find(variable_name) == variables.end())
		{
			//std::cerr << "Unable to solve the texture variable: " + texture_id << std::endl;
			return false;
		}
		texture_id = variables[variable_name];
	}

	if (assets->textures.find(texture_id) == assets->textures.end())
	{
		//std::cerr << "Unable to find texture: " << texture_id << std::endl;
		return false;
	}

	element.faces.push_back(
		Model_Face{
			position,
			assets->textures[texture_id]
		}
	);

	return true;
}

/* Model_Element */

bool load_model_element(Model& model, rapidjson::Value::Object& json, std::unordered_map<std::string, std::string>& variables, Assets* assets)
{
	Model_Element element;

	auto json_from = json["from"].GetArray();
	element.from = {
		json_from[0].GetFloat(),
		json_from[1].GetFloat(),
		json_from[2].GetFloat()
	};

	auto json_to = json["to"].GetArray();
	element.to = {
		json_to[0].GetFloat(),
		json_to[1].GetFloat(),
		json_to[2].GetFloat()
	};

	for (auto& member : json["faces"].GetObject())
	{
		if (!load_model_face(element, member.name.GetString(), member.value.GetObject(), variables, assets))
		{
			return false;
		}
	}

	model.elements.push_back(element);
	return true;
}

/* Model */

bool load_model(std::filesystem::path& base_path, std::string id, Model& model, std::unordered_map<std::string, std::string>& variables, Assets* assets)
{
	// Reads the cached JSON associated with the given path.
	// We don't need the rapidjson::Document instance but its rapidjson::Value::Object equivalent.
	auto json = read_json(base_path / (id + ".json"))->GetObject();

	if (json.HasMember("textures"))
	{
		for (auto& member : json["textures"].GetObject())
		{
			if (member.name.GetString() != "particle"); // We don't care about particle effect.
				variables.insert(std::make_pair(member.name.GetString(), member.value.GetString()));
		}
	}

	if (json.HasMember("elements"))
	{
		for (auto& element : json["elements"].GetArray())
		{
			if (!load_model_element(model, element.GetObject(), variables, assets))
			{
				// An element failed to load, probably because of an undefined texture variable.
				return false;
			}
		}
	}

	if (json.HasMember("parent"))
	{
		// If the parent failed because of an undefined texture variable, also the child has to fail.
		return load_model(base_path, json["parent"].GetString(), model, variables, assets);
	}

	return true; // If no parent is present and the elements successfully loaded, returns true.
}

void load_models(std::filesystem::path base_path, Assets* assets)
{
	auto t0 = std::chrono::high_resolution_clock::now();

	for (const auto& entry : std::filesystem::directory_iterator(base_path / "block"))
	{
		const std::string id = "block/" + entry.path().stem().u8string();

		auto model = std::make_shared<Model>();

		if (!load_model(base_path, id, *model, std::unordered_map<std::string, std::string>(), assets))
		{
			continue; // Intermediate model, like cube.json, we can't load it.
		}

		assets->models.insert(std::make_pair(id, model));
	}

	auto t1 = std::chrono::high_resolution_clock::now();
	std::cout << "Loaded " << assets->models.size() << " models in: " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << "ms" << std::endl;
}

/****************************************************************** block_states */

void BlockState_Variant::from_json(const std::string& id, rapidjson::Value::Object& json, Assets* assets)
{
	BlockState_Variant variant = {
		id,
		assets->models[json["model"].GetString()]
	};

	if (json.HasMember("x"))
		variant.x = json["x"].GetInt();

	if (json.HasMember("y"))
		this->y = json["y"].GetInt();
}

inline void BlockState_Variant::from_json(const std::string& id, rapidjson::Value::Array& json, Assets* assets)
{
	return this->from_json(id, json[0].GetObject(), assets);
}

void BlockState::from_file(const std::filesystem::path& base_path, const std::string& id, Assets* assets)
{
 	this->id = id;

	auto json = read_json(base_path / (id + ".json"))->GetObject();

	if (json.HasMember("variants"))
	{
		for (auto& member : json["variants"].GetObject())
		{
			BlockState_Variant variant;

			if (member.value.IsArray()) // More than one model for the same variant, we'll pick the first one.
				variant.from_json(member.name.GetString(), member.value.GetArray(), assets);
			else
				variant.from_json(member.name.GetString(), member.value.GetObject(), assets);

			this->variants.push_back(std::move(variant));
		}
	}
}

void load_block_states(const std::filesystem::path path, Assets* assets)
{
	auto t0 = std::chrono::high_resolution_clock::now();

	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		const std::string id = entry.path().stem().u8string();

		auto block_state = std::make_shared<BlockState>();
		block_state->from_file(path, id, assets);

		assets->block_states.insert(std::make_pair(id, block_state));
	}

	auto t1 = std::chrono::high_resolution_clock::now();
	std::cout << "Loaded " << assets->models.size() << " block_states in: " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << "ms" << std::endl;
}

const Assets* Minecraft::load(std::filesystem::path path)
{
	Assets* assets = new Assets();

	path /= std::filesystem::path("assets") / "minecraft";

	load_textures(path / "textures", assets);
	load_models(path / "models", assets);
	load_block_states(path / "blockstates", assets);

	json_cache.clear(); // We don't need this dirty cache anymore. TODO clean me.

	return assets;
}
