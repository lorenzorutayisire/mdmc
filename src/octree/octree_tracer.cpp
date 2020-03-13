#include "octree_tracer.hpp"

#include <glm/gtc/type_ptr.hpp>

OctreeRenderer::OctreeRenderer()
{
	// Program
	Shader screen_quad(GL_VERTEX_SHADER);
	screen_quad.source_from_file("resources/shaders/screen_quad.vert");
	if (!screen_quad.compile())
		throw;

	this->program.attach_shader(screen_quad);

	Shader svo_tracer(GL_FRAGMENT_SHADER);
	svo_tracer.source_from_file("resources/shaders/svo_tracer.frag");
	if (!svo_tracer.compile())
		throw;

	this->program.attach_shader(svo_tracer);

	if (!this->program.link())
		throw;
}

void OctreeRenderer::render(glm::uvec2 screen_size, const tdogl::Camera& camera, const std::shared_ptr<Octree>& octree)
{
	this->program.use();

	glUniform1i(this->program.get_uniform_location("uWidth"), screen_size.x);
	glUniform1i(this->program.get_uniform_location("uHeight"), screen_size.y);

	glUniformMatrix4fv(this->program.get_uniform_location("uProjection"), 1, GL_FALSE, glm::value_ptr(camera.projection()));
	glUniformMatrix4fv(this->program.get_uniform_location("uView"), 1, GL_FALSE, glm::value_ptr(camera.view()));
	glUniform3fv(this->program.get_uniform_location("uPosition"), 1, glm::value_ptr(camera.position()));

	octree->bind(3);

	this->screen_quad.render();

	this->program.unuse();
}

