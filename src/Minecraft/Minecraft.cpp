#include "Minecraft.hpp"

#include <filesystem>
#include <stb_image.h>

#include <fstream>

// Just for debug purposes.
#include <iostream>
#include <chrono>

using namespace Minecraft;

std::unordered_map<std::string, nlohmann::json> json_cache;

nlohmann::json& read_json(const std::filesystem::path& path)
{
	std::string path_str = path.u8string();
	if (json_cache.find(path_str) == json_cache.end())
	{
		nlohmann::json json;
		std::ifstream(path) >> json; // Bottleneck!
		json_cache.insert(std::make_pair(path_str, json));
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
		assets->textures.insert(std::make_pair(id, Texture{ data }));
	}

	auto t1 = std::chrono::high_resolution_clock::now();
	std::cout << "Loaded " << assets->textures.size() << " textures in: " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << "ms" << std::endl;
}

/****************************************************************** models */

/* Model_Face */

bool load_model_face(Model_Element& element, std::string position, nlohmann::json& json, std::unordered_map<std::string, std::string>& variables, Assets* assets)
{
	std::string texture_id;

	texture_id = json["texture"].get<std::string>();
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
			&assets->textures[texture_id]
		}
	);

	return true;
}

/* Model_Element */

bool load_model_element(Model& model, nlohmann::json& json, std::unordered_map<std::string, std::string>& variables, Assets* assets)
{
	Model_Element element{
		json["from"].get<std::array<int32_t, 3>>(),
		json["to"].get<std::array<int32_t, 3>>()
	};

	for (auto& [key, value] : json["faces"].items())
	{
		if (!load_model_face(element, key, value, variables, assets))
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
	nlohmann::json json = read_json(base_path / (id + ".json"));

	if (!json["textures"].is_null())
	{
		std::unordered_map<std::string, std::string> local_variables = json["textures"].get<std::unordered_map<std::string, std::string>>();
		local_variables.erase("particle"); // We don't care about particle effect.
		variables.insert(local_variables.begin(), local_variables.end());
	}

	for (auto& [key, value] : json["elements"].items())
	{
		if (!load_model_element(model, value, variables, assets))
		{
			// An element failed to load, probably because of an undefined texture variable.
			return false;
		}
	}

	if (!json["parent"].is_null())
	{
		// If the parent failed because of an undefined texture variable, also the child has to fail.
		return load_model(base_path, json["parent"], model, variables, assets);
	}

	return true; // If no parent is present and the elements successfully loaded, returns true.
}

void load_models(std::filesystem::path base_path, Assets* assets)
{
	auto t0 = std::chrono::high_resolution_clock::now();

	for (const auto& entry : std::filesystem::directory_iterator(base_path / "block"))
	{
		const std::string id = "block/" + entry.path().stem().u8string();

		Model model;
		if (!load_model(base_path, id, model, std::unordered_map<std::string, std::string>(), assets))
		{
			continue; // Intermediate model, like cube.json, we can't load it.
		}

		assets->models.insert(std::make_pair(id, model));
	}

	auto t1 = std::chrono::high_resolution_clock::now();
	std::cout << "Loaded " << assets->models.size() << " models in: " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << "ms" << std::endl;
}

/****************************************************************** block_states */

BlockState_Variant load_block_state_variant(const std::string& id, nlohmann::json& json, Assets* assets)
{
	// If the current variant has more than one model, picks the first one (Minecraft picks it randomly).
	if (json.is_array())
	{
		json = json[0];
	}

	BlockState_Variant variant = {
		id,
		&assets->models[json["model"].get<std::string>()]
	};

	if (!json["x"].is_null())
		variant.x = json["x"].get<int32_t>();

	if (!json["y"].is_null())
		variant.y = json["y"].get<int32_t>();

	return variant;
}

BlockState load_block_state(const std::filesystem::path& base_path, const std::string& id, Assets* assets)
{
	nlohmann::json json = read_json(base_path / (id + ".json"));

	BlockState block_state{ id };

	for (auto& [key, value] : json["variants"].items())
	{
		block_state.variants.push_back(
			load_block_state_variant(key, value, assets)
		);
	}

	return block_state;
}

void load_block_states(const std::filesystem::path path, Assets* assets)
{
	auto t0 = std::chrono::high_resolution_clock::now();

	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		const std::string id = entry.path().stem().u8string();
		BlockState block_state = load_block_state(path, id, assets);

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
