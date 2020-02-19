#include "voxelizer.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <iostream>
#include <fstream>

#include "debug.hpp"

#include <chrono>

#define DEBUG

// ================================================================================================

Voxelizer::Voxelizer() :
	program(Program::create())
{
	// Vertex
	auto v_shader = Shader::create(GL_VERTEX_SHADER);
	v_shader.source_from_file("resources/shaders/voxelize.vert");
	if (!v_shader.compile())
	{
		std::cerr << v_shader.get_log() << std::endl;
		throw;
	}
	this->program.attach_shader(v_shader);

	// Geometry
	auto g_shader = Shader::create(GL_GEOMETRY_SHADER);
	g_shader.source_from_file("resources/shaders/voxelize.geom");
	if (!g_shader.compile())
	{
		std::cerr << g_shader.get_log() << std::endl;
		throw;
	}
	this->program.attach_shader(g_shader);

	// Fragment
	auto f_shader = Shader::create(GL_FRAGMENT_SHADER);
	f_shader.source_from_file("resources/shaders/voxelize.frag");
	if (!f_shader.compile())
	{
		std::cerr << f_shader.get_log() << std::endl;
		throw;
	}
	this->program.attach_shader(f_shader);

	// Program
	if (!this->program.link())
	{
		std::cerr << this->program.get_log() << std::endl;
		throw;
	}
}

template<typename T>
T round_to_2p(T n)
{
	n--;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	n++;

	return n;
}

std::shared_ptr<const Volume> Voxelizer::voxelize(const std::shared_ptr<const Field>& field, glm::uvec3 grid)
{
	glEnable(GL_CONSERVATIVE_RASTERIZATION_NV);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	// The viewport side is the largest side of the grid.
	glm::uint viewport = glm::max(grid.x, glm::max(grid.y, grid.z));
	glViewport(0, 0, viewport, viewport);

	auto framebuffer = FrameBuffer::create();
	framebuffer.set_default_size(viewport, viewport);

	this->program.use();

	glUniformMatrix4fv(this->program.get_uniform_location("u_transform"), 1, GL_FALSE, glm::value_ptr(field->get_pivot()));

	glUniformMatrix4fv(this->program.get_uniform_location("u_x_ortho_projection"), 1, GL_FALSE, glm::value_ptr(field->get_x_ortho()));
	glUniformMatrix4fv(this->program.get_uniform_location("u_y_ortho_projection"), 1, GL_FALSE, glm::value_ptr(field->get_y_ortho()));
	glUniformMatrix4fv(this->program.get_uniform_location("u_z_ortho_projection"), 1, GL_FALSE, glm::value_ptr(field->get_z_ortho()));

	glUniform1f(this->program.get_uniform_location("u_viewport_side"), (GLfloat) viewport);
	glUniform3f(this->program.get_uniform_location("u_volume_size"), field->size.x, field->size.y, field->size.z);

	glUniform1ui(this->program.get_uniform_location("can_store"), 1);

	framebuffer.use();

	field->render();

	framebuffer.unuse();

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	return nullptr;
}
