#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include <filesystem>
#include <nlohmann/json.hpp>


namespace Minecraft
{
	/*********************************************************************** Texture */

	struct Texture
	{
		uint8_t* data;
	};

	/*********************************************************************** Model */

	struct Model_Face
	{
		std::string position; // down, up, north, south, west, east
		Texture* texture;
		// rotation
		// tintindex
	};

	struct Model_Element
	{
		std::array<int32_t, 3> from;
		std::array<int32_t, 3> to;
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
		Model* model;
		int32_t x;
		int32_t y;
	};

	struct BlockState
	{
		std::string id; // Actually the filename without .json extension.
		std::vector<BlockState_Variant> variants;
	};

	/*********************************************************************** Asset */

	struct Assets
	{
		std::unordered_map<std::string, Texture> textures;
		std::unordered_map<std::string, Model> models;
		std::unordered_map<std::string, BlockState> block_states;
	};

	const Assets* load(const std::filesystem::path path);
}
