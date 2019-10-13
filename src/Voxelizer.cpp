#include "Voxelizer.hpp"

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <iostream>

Voxelizer::Voxelizer(Scene& scene, uint16_t height)
{
	this->scene = std::make_shared<Scene>(scene);

	float scalar = 1 / scene.get_size().y;

	uint16_t width = scalar * scene.get_size().x * height;
	uint16_t depth = scalar * scene.get_size().z * height;
	this->side = std::max(width, std::max(depth, height));

	// Transform
	this->transform = glm::mat4(1.0);
	this->transform = glm::scale(this->transform, glm::vec3(scalar));
	this->transform = glm::translate(this->transform, -this->scene->get_min_vertex());

	// Projections
	glm::mat4 ortho = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 2.0f);
	this->x_ortho_projection = ortho * glm::lookAt(glm::vec3(-1, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	this->y_ortho_projection = ortho * glm::lookAt(glm::vec3(0, -1, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
	this->z_ortho_projection = ortho * glm::lookAt(glm::vec3(0, 0, 2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	// Texture3D
	glGenTextures(1, &this->voxel);
	glBindTexture(GL_TEXTURE_3D, this->voxel);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, this->side, this->side, this->side, 0, GL_RGBA, GL_FLOAT, NULL);
	glClearTexImage(this->voxel, 0, GL_RGBA, GL_FLOAT, new float[4]{ 0, 0, 0, 0 });

	// Vertex
	Shader v_shader(GL_VERTEX_SHADER);
	v_shader.source_from_file("resources/shaders/voxelize.vert.glsl");
	if (!v_shader.compile())
	{
		std::cerr << v_shader.get_log() << std::endl;
		throw;
	}
	this->program.attach(v_shader);

	// Geometry
	Shader g_shader(GL_GEOMETRY_SHADER);
	g_shader.source_from_file("resources/shaders/voxelize.geom.glsl");
	if (!g_shader.compile())
	{
		std::cerr << g_shader.get_log() << std::endl;
		throw;
	}
	this->program.attach(g_shader);

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

void Voxelizer::voxelize()
{
	glEnable(GL_TEXTURE_3D);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	//glfwSetWindowSize(window, width, height);
	glViewport(0, 0, this->side, this->side);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClearColor(0, 0, 0, 0);

	this->program.use();

	// Transform
	glUniformMatrix4fv(this->program.get_uniform_location("u_transform"), 1, GL_FALSE, glm::value_ptr(this->transform));

	// Projections
	glUniformMatrix4fv(this->program.get_uniform_location("u_x_ortho_projection"), 1, GL_FALSE, glm::value_ptr(this->x_ortho_projection));
	glUniformMatrix4fv(this->program.get_uniform_location("u_y_ortho_projection"), 1, GL_FALSE, glm::value_ptr(this->y_ortho_projection));
	glUniformMatrix4fv(this->program.get_uniform_location("u_z_ortho_projection"), 1, GL_FALSE, glm::value_ptr(this->z_ortho_projection));

	// Voxel Size
	glUniform1f(this->program.get_uniform_location("u_voxel_size"), this->side);

	// Voxel
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_3D, voxel); // important!
	glBindImageTexture(5, voxel, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);

	for (Mesh mesh : scene->get_meshes())
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh.get_texture());

		glUniform4f(this->program.get_uniform_location("u_color"), mesh.get_color().r, mesh.get_color().g, mesh.get_color().b, mesh.get_color().a);

		glBindVertexArray(mesh.get_vao());

		glDrawElements(GL_TRIANGLES, mesh.get_elements_count(), GL_UNSIGNED_INT, NULL);
	}

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

