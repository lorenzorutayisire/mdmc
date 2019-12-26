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

		struct Texture
		{
			uint32_t x, y;
			uint32_t w, h;
		};

		struct Atlas
		{
			uint32_t w, h;
			std::unordered_map<std::string, Texture> textures_by_name;
		};

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
			std::string name;
			uint32_t vertices_offset, vertices_count;
		};

		// =====================================================================================
		// Assets
		// =====================================================================================

		class Assets
		{
		private:
			std::filesystem::path base_path;
			std::string version;

			GLuint atlas;

			std::vector<Block> blocks_by_name;
			GLuint vbo;
			uint32_t vertices_count;

			Atlas load_atlas_descriptor();
			void load_atlas();

			int load_model_face(
				const std::unordered_map<std::string, std::string>& textures_variables,
				const Atlas& atlas,

				ModelFaceOrientation face_orientation,
				const ModelFace& face,
				const ModelElement& element,

				glm::mat4 transformation,
				std::vector<GLfloat>& vertices
			);

			int load_model_element(
				const std::unordered_map<std::string, std::string>& variables,
				const Atlas& atlas,

				const ModelElement& element,

				glm::mat4 transformation,
				std::vector<GLfloat>& vertices
			);

			int load_model(
				const std::string& model_id,

				std::unordered_map<std::string, std::string>& textures_variables,
				const Atlas& atlas,

				glm::mat4 transformation,
				std::vector<GLfloat>& vertices,

				bool can_store = true
			);

			void load_block_states(
				const Atlas& atlas,
				bool forced = false
			);

		public:
			Assets(
				const std::filesystem::path& base_path,
				const std::string& version
			);

			const std::filesystem::path path() const;

			const Block& get_block(size_t id) const;
			const std::vector<Block>& get_blocks() const;

			GLuint get_atlas() const;
			GLuint get_vbo() const;
			size_t get_vertices_count() const;

			void retrieve();
			void load();

			void render();
		};
	}
}
