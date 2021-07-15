#include "minecraft_renderer.hpp"

#include <iostream>

#include <glm/gtc/type_ptr.hpp>

#ifdef IGNORED

void MinecraftRenderer::render(
	glm::mat4 const& camera,
	glm::mat4 const& transform,
	glm::vec4 const& tint,
	std::shared_ptr<MinecraftContext const> const& context,
	std::function<void()> const& draw_call
)
{
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	this->program.use();

	glUniformMatrix4fv(this->program.get_uniform_location("u_camera"), 1, GL_FALSE, glm::value_ptr(camera));
	glUniformMatrix4fv(this->program.get_uniform_location("u_transform"), 1, GL_FALSE, glm::value_ptr(transform));

	glUniform4fv(this->program.get_uniform_location("u_tint_color"), 1, glm::value_ptr(tint));

	glBindTexture(GL_TEXTURE_2D, context->get_atlas_texture());

	draw_call();

	glBindTexture(GL_TEXTURE_2D, 0);
	this->program.unuse();
}

MinecraftRenderer::MinecraftRenderer()
{
	Shader vert(GL_VERTEX_SHADER);
	vert.source_from_file("resources/shaders/mc_render.vert");
	if (!vert.compile())
		throw;

	this->program.attach_shader(vert);

	Shader frag(GL_FRAGMENT_SHADER);
	frag.source_from_file("resources/shaders/mc_render.frag");
	if (!frag.compile())
		throw;

	this->program.attach_shader(frag);

	if (!this->program.link())
	{
		std::cerr << this->program.get_log() << std::endl;
		throw;
	}
}

void MinecraftRenderer::render_block(
	glm::mat4 const& camera,
	glm::mat4 const& transform,
	glm::vec4 const& tint,
	std::shared_ptr<MinecraftContext const> const& context,
	MinecraftBakedBlock const& baked_block
)
{
	this->render(camera, transform, tint, context,[&] {
		baked_block.draw();
	});
}

void MinecraftRenderer::render_world(
	glm::mat4 const& camera,
	glm::mat4 const& transform,
	glm::vec4 const& tint,
	std::shared_ptr<MinecraftContext const> const& context,
	std::shared_ptr<MinecraftWorld> const& world
)
{
	this->render(camera, transform, tint, context, [&] {
		world->draw();
	});
}
#endif
