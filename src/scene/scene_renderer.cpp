#include "scene_renderer.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <sstream>

SceneRenderer::SceneRenderer()
{
	Shader scene_vert(GL_VERTEX_SHADER);
	scene_vert.source_from_file("resources/shaders/scene.vert");
	if (!scene_vert.compile())
		throw;

	this->program.attach_shader(scene_vert);

	Shader scene_frag(GL_FRAGMENT_SHADER);
	scene_frag.source_from_file("resources/shaders/phong_scene.frag");
	if (!scene_frag.compile())
		throw;

	this->program.attach_shader(scene_frag);

	this->program.link();
}

void SceneRenderer::render(const glm::mat4& camera, const glm::mat4& transform, const std::shared_ptr<Scene>& scene, Material::Type view_type)
{
	glEnable(GL_DEPTH_TEST);

	this->program.use();

	glUniformMatrix4fv(this->program.get_uniform_location("u_camera"), 1, GL_FALSE, glm::value_ptr(camera));
	glUniformMatrix4fv(this->program.get_uniform_location("u_scene_transform"), 1, GL_FALSE, glm::value_ptr(transform));

	for (auto& mesh : scene->meshes)
	{
		glUniformMatrix4fv(this->program.get_uniform_location("u_transform"), 1, GL_FALSE, glm::value_ptr(mesh->transform));

		glUniform4fv(this->program.get_uniform_location("u_color"), 1, glm::value_ptr(mesh->material->get_color(view_type)));
		glBindTexture(GL_TEXTURE_2D, mesh->material->get_texture(view_type));

		glBindVertexArray(mesh->vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);

		glDrawElements(GL_TRIANGLES, mesh->elements_count, GL_UNSIGNED_INT, NULL);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	this->program.unuse();
}
