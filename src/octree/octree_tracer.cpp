#include "octree_tracer.hpp"

#include <iostream>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

OctreeRenderer::OctreeRenderer()
{
	bool result;

	// Program
	Shader screen_quad(GL_VERTEX_SHADER);
	screen_quad.source_from_file("resources/shaders/screen_quad.vert");
	if (!screen_quad.compile())
	{
		std::cerr << "Error during shader compilation: " << screen_quad.get_log() << std::endl;
		throw;
	}

	this->program.attach_shader(screen_quad);

	Shader svo_tracer(GL_FRAGMENT_SHADER);
	svo_tracer.source_from_file("resources/shaders/svo_tracer.frag");
	result = svo_tracer.compile();
	std::cout << svo_tracer.get_log() << std::endl;
	if (!result)
		throw;

	this->program.attach_shader(svo_tracer);

	result = this->program.link();
	std::cout << this->program.get_log() << std::endl;
	if (!result)
		throw;
}

void OctreeRenderer::render(
	glm::uvec2 const& screen,

	glm::vec3 const& position,
	glm::vec3 const& size,

	glm::mat4 const& camera_projection,
	glm::mat4 const& camera_view,
	glm::vec3 const& camera_position,
	std::shared_ptr<Octree> const& octree
)
{
	this->program.use();

	glUniform3fv(this->program.get_uniform_location("u_octree_from"), 1, glm::value_ptr(position));
	glUniform3fv(this->program.get_uniform_location("u_octree_size"), 1, glm::value_ptr(size));

	glUniform2uiv(this->program.get_uniform_location("u_screen"), 1, glm::value_ptr(screen));

	// camera
	glUniformMatrix4fv(this->program.get_uniform_location("u_projection"), 1, GL_FALSE, glm::value_ptr(camera_projection));
	glUniformMatrix4fv(this->program.get_uniform_location("u_view"), 1, GL_FALSE, glm::value_ptr(camera_view));
	glUniform3fv(this->program.get_uniform_location("u_position"), 1, glm::value_ptr(camera_position));

	octree->bind(0);

	this->screen_quad.render();

	this->program.unuse();
}

