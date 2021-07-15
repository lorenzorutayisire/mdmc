#pragma once

#include <memory>
#include <filesystem>
#include <functional>
#include <map>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <zip.h>
#include <rapidjson/document.h>

namespace mdmc
{
	class mc_assets;
	struct mc_model_element;

	struct mc_atlas
	{
		struct texture
		{
			int m_x;
			int m_y;
			int m_width;
			int m_height;

			inline float u() const {
				return m_x / (float) m_width;
			}

			inline float v() const {
				return m_y / (float) m_height;
			}
		};

		size_t m_entry_width;
		size_t m_entry_height;

		std::map<std::string, texture> m_textures;

		std::vector<uint8_t> m_data;

		inline size_t entry_size() const {
			return m_entry_width * m_entry_height;
		}

		inline size_t width() const {
			return m_entry_width * m_textures.size();
		}

		inline size_t height() const {
			return m_entry_height;
		}

		inline size_t size() const {
			return entry_size() * m_textures.size();
		}
	};

	enum class mc_model_element_face_orientation { WEST, EAST, DOWN, UP, SOUTH, NORTH, Count };

	struct mc_model_element_face
	{
		mc_model_element_face_orientation m_orientation;

		glm::vec2 m_from_uv, m_to_uv;
		std::string m_texture;
		//bool m_cull_face;
		unsigned int m_rotation;
		bool m_tint_index;

		mdmc::mc_atlas::texture const& mdmc::mc_model_element_face::get_texture(
			mdmc::mc_assets const& assets,
			std::unordered_map<std::string, std::string> const& texture_by_variable
		) const;

		void from_json(mdmc::mc_model_element& element, rapidjson::Value::Object const& json);
	};

	struct mc_model_element
	{
		glm::vec3 m_from, m_to;
		std::unordered_map<mc_model_element_face_orientation, mc_model_element_face> m_face_by_position;

		struct {
			glm::vec3 m_origin;
			char m_axis;
			float m_angle;
			bool m_rescale;
		} m_rotation;

		void from_json(rapidjson::Value::Object const& json);
	};

	struct mc_model
	{
		std::string m_parent_model;
		std::unordered_map<std::string, std::string> m_texture_by_variable;
		std::vector<mc_model_element> m_elements;

		void from_json(rapidjson::Value::Object const& json);
	};

	// ------------------------------------------------------------------------------------------------ mc_block_state

	struct mc_block_state_variant
	{
		std::string m_model;
		unsigned int m_x, m_y;
		bool m_uvlock;

		void from_json(rapidjson::Value::Object const& json);
	};

	struct mc_block_state
	{
		std::map<std::string, mdmc::mc_block_state_variant> m_variant_by_name;

		void from_json(rapidjson::Value::Object const& json);
	};

	struct mc_assets
	{
		std::string m_version;

		mdmc::mc_atlas m_atlas;
		std::map<std::string, mdmc::mc_model> m_model_by_name;
		std::map<std::string, mdmc::mc_block_state> m_block_state_by_name;
		std::map<std::string, std::reference_wrapper<mdmc::mc_block_state_variant>> m_block_state_variant_by_name;

		void from_jar(zip* mc_jar, std::string const& mc_version);
	};

}
