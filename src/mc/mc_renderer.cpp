#include "mc_renderer.hpp"

#include <iostream>

#include <glm/gtc/type_ptr.hpp>
#include <utility>

#include "mc_baker.hpp"

// mc_baked_world

mdmc::mc_baked_world::mc_baked_world(std::shared_ptr<mdmc::mc_assets> const& assets) :
	m_assets(assets)
{
	glGenTextures(1, &m_atlas_texture);
	mdmc::bake_mc_atlas(m_atlas_texture, assets->m_atlas);

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
}

mdmc::mc_baked_world::~mc_baked_world()
{
	glDeleteTextures(1, &m_atlas_texture);

	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);
}

void mdmc::mc_baked_world::set_block(glm::ivec3 const& position, uint32_t block_id)
{
	m_block_by_position[position] = block_id;
}

void mdmc::mc_baked_world::rebuild()
{
	std::vector<float> buffer;
	m_vertices_count = 0;

	for (auto const& [position, block_id] : m_block_by_position)
	{
		auto& block_state_variant = m_assets->m_block_state_variant_by_id.at(block_id).second;

		glm::mat4 transform = glm::translate(glm::identity<glm::mat4>(), glm::vec3(position));
		m_vertices_count += (int32_t) mdmc::bake_mc_block_state_variant(*m_assets, block_state_variant, transform, buffer);
	}

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) (buffer.size() * sizeof(float)), buffer.data(), GL_STATIC_DRAW);

	mdmc::define_block_vertex_layout(m_vao);
}

// mc_renderer

mdmc::mc_renderer::mc_renderer()
{
	Shader vert(GL_VERTEX_SHADER);
	vert.source_from_file("resources/shaders/mc_render.vert");
	if (!vert.compile())
		throw;

	m_program.attach_shader(vert);

	Shader frag(GL_FRAGMENT_SHADER);
	frag.source_from_file("resources/shaders/mc_render.frag");
	if (!frag.compile())
		throw;

	m_program.attach_shader(frag);

	if (!m_program.link())
	{
		std::cerr << m_program.get_log() << std::endl;
		throw;
	}
}

mdmc::mc_renderer::~mc_renderer()
{}

void mdmc::mc_renderer::render(mdmc::mc_baked_world const& world, glm::mat4 const& camera, glm::mat4 const& transform)
{
	if (world.m_vertices_count == 0) {
		return;
	}

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_program.use();

	glUniformMatrix4fv(m_program.get_uniform_location("u_camera"), 1, GL_FALSE, glm::value_ptr(camera));
	glUniformMatrix4fv(m_program.get_uniform_location("u_transform"), 1, GL_FALSE, glm::value_ptr(transform));

	glm::vec4 tint_color = glm::vec4(0, 1, 0, 1);
	glUniform4fv(m_program.get_uniform_location("u_tint_color"), 1, glm::value_ptr(tint_color));

	glBindTexture(GL_TEXTURE_2D, world.m_atlas_texture);

	glBindVertexArray(world.m_vao);
	glDrawArrays(GL_QUADS, 0, world.m_vertices_count);

	Program::unuse();
}
