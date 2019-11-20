#pragma once

#include <memory>
#include <array>
#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>

#include <rapidjson/document.h>

namespace Minecraft
{
	struct Assets; // Pre-definition

	/*********************************************************************** Texture */

	struct Texture
	{
		uint8_t* data;
	};

	/*********************************************************************** Model */

	struct Model_Face
	{
		std::string position; // down, up, north, south, west, east
		std::shared_ptr<Texture> texture;
		// rotation
		// tintindex
	};

	struct Model_Element
	{
		std::array<float, 3> from;
		std::array<float, 3> to;
		// rotation
		std::vector<Model_Face> faces;
	};

	struct Model
	{
		std::shared_ptr<Model> parent;
		std::vector<Model_Element> elements;
	};

	/*********************************************************************** BlockState */

	struct BlockState_Variant
	{
		std::string id; // The name of the single variant.
		std::shared_ptr<Model> model;
		int32_t x;
		int32_t y;

		void from_json(const std::string& id, rapidjson::Value::Object& json, Assets* assets);
		void from_json(const std::string& id, rapidjson::Value::Array& json, Assets* assets);
	};

	struct BlockState
	{
		std::string id; // Actually the filename without .json extension.
		std::vector<BlockState_Variant> variants;
	
		void from_file(const std::filesystem::path& base_path, const std::string& id, Assets* assets);
	};

	/*********************************************************************** Asset */

	struct Assets
	{
		std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
		std::unordered_map<std::string, std::shared_ptr<Model>> models;
		std::unordered_map<std::string, std::shared_ptr<BlockState>> block_states;
	};

	const Assets* load(const std::filesystem::path path);
}
