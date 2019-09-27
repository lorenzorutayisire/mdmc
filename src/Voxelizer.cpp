#include "Voxelizer.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

Voxelizer::Voxelizer(uint16_t width, uint16_t height, uint16_t depth) :
	width(width),
	height(height),
	depth(depth),
	size(width * height * depth)
{
	GLenum error;

	// Texture3D
	glGenTextures(1, &voxel);
	glBindTexture(GL_TEXTURE_3D, voxel);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, this->width, this->height, this->depth, 0, GL_RGBA, GL_FLOAT, NULL);
	glClearTexImage(voxel, 0, GL_RGBA8, GL_FLOAT, new float[4]{ 0, 0, 0, 0 });

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	/* Program */

	// Vertex
	Shader v_shader(GL_VERTEX_SHADER);
	v_shader.source_from_file("resources/shaders/voxelize.vert.glsl");
	if (!v_shader.compile())
	{
		std::cerr << v_shader.get_log() << std::endl;
		throw;
	}
	this->program.attach(v_shader);

	// Fragment
	Shader f_shader(GL_FRAGMENT_SHADER);
	f_shader.source_from_file("resources/shaders/voxelize.frag.glsl");
	if (!f_shader.compile())
	{
		std::cerr << f_shader.get_log() << std::endl;
		throw;
	}
	this->program.attach(f_shader);

	// Program
	if (!this->program.link())
	{
		std::cerr << this->program.get_log() << std::endl;
		throw;
	}
}

Voxelizer::~Voxelizer()
{
}

void Voxelizer::voxelize(Scene& scene)
{
	glViewport(0, 0, this->width, this->height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	// glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	this->program.use();

	// Transform
	glm::mat4 transform = glm::mat4(1.0);
	transform = glm::scale(transform, glm::vec3(1) / (scene.get_max_vertex() - scene.get_min_vertex()));
	transform = glm::translate(transform, -scene.get_min_vertex());
	glUniformMatrix4fv(this->program.get_uniform_location("u_transform"), 1, GL_FALSE, glm::value_ptr(transform));

	// Camera
	glm::mat4 camera = glm::mat4(1.0);
	camera = glm::ortho(0, 1, 0, 1, -1, 0);
	glUniformMatrix4fv(this->program.get_uniform_location("u_camera"), 1, GL_FALSE, glm::value_ptr(camera));

	// Voxel Size
	glUniform3i(this->program.get_uniform_location("u_voxel_size"), this->width, this->height, this->depth);

	// Voxel
	glBindImageTexture(1, voxel, 0, GL_FALSE, GL_TRUE, GL_READ_WRITE, GL_RGBA8);

	for (Mesh mesh : scene.get_meshes())
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh.get_texture());

		glUniform4f(this->program.get_uniform_location("u_color"), mesh.get_color().r, mesh.get_color().g, mesh.get_color().b, mesh.get_color().a);

		glBindVertexArray(mesh.get_vao());
		glDrawElements(GL_TRIANGLES, mesh.get_elements_count(), GL_UNSIGNED_INT, NULL);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}
}

