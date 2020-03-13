#include "scene_voxelizer.hpp"

#include <iostream>

#include <glm/gtc/type_ptr.hpp>

#include "scene/scene.hpp"
#include "util/gl.hpp"

SceneVoxelizer::SceneVoxelizer()
{
	// Vertex
	Shader vertex(GL_VERTEX_SHADER);
	vertex.source_from_file("resources/shaders/voxelize.vert");
	if (!vertex.compile())
		throw;

	this->program.attach_shader(vertex);

	// Geometry
	Shader geometry(GL_GEOMETRY_SHADER);
	geometry.source_from_file("resources/shaders/voxelize.geom");
	if (!geometry.compile())
		throw;

	this->program.attach_shader(geometry);

	// Fragment
	Shader fragment(GL_FRAGMENT_SHADER);
	fragment.source_from_file("resources/shaders/voxelize.frag");
	if (!fragment.compile())
		throw;

	this->program.attach_shader(fragment);

	if (!this->program.link())
		throw;
}

// TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void render(Program& program, const Scene& scene)
{
	for (const std::shared_ptr<Mesh>& mesh : scene.meshes)
	{
		glUniformMatrix4fv(program.get_uniform_location("u_transform"), 1, GL_FALSE, glm::value_ptr(mesh->transform));

		//glUniform4fv(program.get_uniform_location("u_color"), 1, glm::value_ptr(mesh->material->color));
		//glBindTexture(GL_TEXTURE_2D, mesh->material->texture);

		glBindVertexArray(mesh->vao);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);

		glDrawElements(GL_TRIANGLES, mesh->elements_count, GL_UNSIGNED_INT, NULL);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

std::shared_ptr<VoxelFragmentList> SceneVoxelizer::voxelize(const glm::uvec3& grid, const Scene& scene)
{
	this->program.use();

	glm::mat4 x_ortho, y_ortho, z_ortho;
	glUniformMatrix4fv(program.get_uniform_location("u_x_ortho_projection"), 1, GL_FALSE, glm::value_ptr(x_ortho));
	glUniformMatrix4fv(program.get_uniform_location("u_y_ortho_projection"), 1, GL_FALSE, glm::value_ptr(y_ortho));
	glUniformMatrix4fv(program.get_uniform_location("u_z_ortho_projection"), 1, GL_FALSE, glm::value_ptr(z_ortho));

	glm::uint viewport = glm::max(glm::max(grid.x, grid.y), grid.z);
	glUniform1ui(program.get_uniform_location("u_viewport"), viewport);

	glUniform3uiv(program.get_uniform_location("u_grid"), 1, glm::value_ptr(grid));

	auto result = std::make_shared<VoxelFragmentList>();

	/* Count */
	glUniform1ui(program.get_uniform_location("u_can_store"), 0);

	AtomicCounter voxel_count;
	voxel_count.set_value(0);
	voxel_count.bind(1);

	render(program, scene);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);

	//result->count = voxel_count.get_value();

	/* Store */
	glUniform1ui(program.get_uniform_location("u_can_store"), 1);

	//glm::size_t size = result->count * sizeof(Voxel);
	//result->ssbo.load_data(size, NULL, GL_DYNAMIC_DRAW);
	//result->ssbo.bind(2);

	voxel_count.set_value(0);

	render(program, scene);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	this->program.unuse();

	return result;
}
