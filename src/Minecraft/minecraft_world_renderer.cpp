#include "minecraft_world_renderer.hpp"

#include <iostream>

#include <glm/gtc/type_ptr.hpp>

MinecraftWorldRenderer::MinecraftWorldRenderer()
{
	Shader vert(GL_VERTEX_SHADER);
	vert.source_from_file("resources/shaders/minecraft_world.vert");
	if (!vert.compile())
		throw;

	this->program.attach_shader(vert);

	Shader frag(GL_FRAGMENT_SHADER);
	frag.source_from_file("resources/shaders/minecraft_world.frag");
	if (!frag.compile())
		throw;

	this->program.attach_shader(frag);

	if (!this->program.link())
	{
		std::cerr << this->program.get_log() << std::endl;
		throw;
	}
}

void MinecraftWorldRenderer::render(const glm::mat4& camera, const glm::mat4& transform, const std::shared_ptr<MinecraftWorld>& world, const glm::vec4& tint_color)
{
	this->program.use();

	glUniformMatrix4fv(this->program.get_uniform_location("u_camera"), 1, GL_FALSE, glm::value_ptr(camera));
	glUniformMatrix4fv(this->program.get_uniform_location("u_transform"), 1, GL_FALSE, glm::value_ptr(transform));
	glUniform4fv(this->program.get_uniform_location("u_tint_color"), 1, glm::value_ptr(tint_color));

	world->render();

	this->program.unuse();
}
