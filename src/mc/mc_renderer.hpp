#pragma once

#include "util/gl.hpp"

#include "mc_assets.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>

// std::hash<glm::ivec3>
namespace std
{
	template<>
	struct hash<glm::ivec3>
	{
		size_t operator()(glm::ivec3 const& v) const
		{
			return std::hash<int>()(v.x) ^ std::hash<int>()(v.y) ^ std::hash<int>()(v.z);
		}

		bool operator()(glm::ivec3 const& a, glm::ivec3 const& b) const
		{
			return a.x == b.x && a.y == b.y && a.z == b.z;
		}
	};
}

namespace mdmc
{
	struct mc_baked_world
	{
		std::shared_ptr<mdmc::mc_assets> m_assets;
		std::unordered_map<glm::ivec3, uint32_t> m_block_by_position;

		GLuint m_atlas_texture;
		GLuint m_vao, m_vbo;
		int32_t m_vertices_count = 0;

		mc_baked_world(std::shared_ptr<mdmc::mc_assets> const& assets);
		~mc_baked_world();

		void set_block(glm::ivec3 const& position, uint32_t block_id);

		void rebuild();
	};

	class mc_renderer
	{
	private:
		Program m_program;

	public:
		mc_renderer();
		~mc_renderer();

		void render(mdmc::mc_baked_world const& world, glm::mat4 const& camera, glm::mat4 const& transform);
	};
}
