#pragma once

#include <memory>
#include <array>
#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

#include <rapidjson/document.h>

namespace mdmc
{
	namespace Minecraft
	{
		// =====================================================================================
		// Texture
		// =====================================================================================

		using Texture = size_t;

		// =====================================================================================
		// Block
		// =====================================================================================

		enum ModelFaceOrientation
		{
			WEST,
			EAST,
			DOWN,
			UP,
			NORTH,
			SOUTH
		};


		using ModelFace = rapidjson::Value::Object;

		using ModelElement = rapidjson::Value::Object;

		using Model = rapidjson::Value::Object;

		using Variant = rapidjson::Value::Object;

		struct Block
		{
			std::string id;

			size_t start_vertex_offset;
			size_t vertices_count;
		};

		// =====================================================================================
		// Assets
		// =====================================================================================

		struct Assets
		{
			std::unordered_map<std::string, Texture> textures_by_name;
			std::vector<Block> blocks_by_id;

			GLuint textures;

			GLuint vbo;
			size_t vertices_count;

			void store_textures(const std::filesystem::path& path);

			bool store_model_face(
				const std::unordered_map<std::string, std::string>& variables,

				ModelFaceOrientation face_orientation,
				const ModelFace& face,
				const ModelElement& element,

				glm::mat4 transformation,
				std::vector<GLfloat>& vertices
			);

			bool store_model_element(
				const std::unordered_map<std::string, std::string>& variables,

				const ModelElement& element,

				glm::mat4 transformation,
				std::vector<GLfloat>& vertices
			);

			bool store_model(
				const std::filesystem::path& base_path,
				const std::string& id,
				std::unordered_map<std::string, std::string>& variables,

				glm::mat4 transformation,
				std::vector<GLfloat>& vertices,

				bool can_store = true
			);

			void store_blocks(const std::filesystem::path& base_path);

			void store(const std::filesystem::path& base_path);
		};
	}
}
