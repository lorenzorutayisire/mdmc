#pragma once

#include <iostream>

#include "GL/Program.hpp"

#include "Phase.hpp"
#include "Voxelizer.hpp"

class VoxelizePhase : public Phase
{
private:
	GLuint vbo;
	GLuint vertices_count;

	Program program;

	Viewer viewer;

	std::shared_ptr<Voxelizer> voxelizer;

	void push_x_planes(std::vector<GLfloat>& vertices, uint16_t side)
	{
		for (GLfloat i = 0; i <= side; i++)
		{
			float x = i / (GLfloat) side;

			// v0
			vertices.push_back(x);
			vertices.push_back(0);
			vertices.push_back(0);

			vertices.push_back(1);
			vertices.push_back(0);
			vertices.push_back(0);

			// v1
			vertices.push_back(x);
			vertices.push_back(1);
			vertices.push_back(0);

			vertices.push_back(1);
			vertices.push_back(0);
			vertices.push_back(0);

			// v2
			vertices.push_back(x);
			vertices.push_back(1);
			vertices.push_back(1);

			vertices.push_back(1);
			vertices.push_back(0);
			vertices.push_back(0);

			// v3
			vertices.push_back(x);
			vertices.push_back(0);
			vertices.push_back(1);

			vertices.push_back(1);
			vertices.push_back(0);
			vertices.push_back(0);
		}
	}

	void push_y_planes(std::vector<GLfloat>& vertices, uint16_t side)
	{
		for (GLfloat i = 0; i <= side; i++)
		{
			float y = i / (GLfloat) side;

			// v0
			vertices.push_back(0);
			vertices.push_back(y);
			vertices.push_back(0);

			vertices.push_back(0);
			vertices.push_back(1);
			vertices.push_back(0);

			// v1
			vertices.push_back(1);
			vertices.push_back(y);
			vertices.push_back(0);

			vertices.push_back(0);
			vertices.push_back(1);
			vertices.push_back(0);

			// v2
			vertices.push_back(1);
			vertices.push_back(y);
			vertices.push_back(1);

			vertices.push_back(0);
			vertices.push_back(1);
			vertices.push_back(0);

			// v3
			vertices.push_back(0);
			vertices.push_back(y);
			vertices.push_back(1);

			vertices.push_back(0);
			vertices.push_back(1);
			vertices.push_back(0);
		}
	}

	void push_z_planes(std::vector<GLfloat>& vertices, uint16_t side)
	{
		for (GLfloat i = 0; i <= side; i++)
		{
			float z = i / (GLfloat) side;

			// v0
			vertices.push_back(0);
			vertices.push_back(0);
			vertices.push_back(z);

			vertices.push_back(0);
			vertices.push_back(0);
			vertices.push_back(1);

			// v1
			vertices.push_back(1);
			vertices.push_back(0);
			vertices.push_back(z);

			vertices.push_back(0);
			vertices.push_back(0);
			vertices.push_back(1);

			// v2
			vertices.push_back(1);
			vertices.push_back(1);
			vertices.push_back(z);

			vertices.push_back(0);
			vertices.push_back(0);
			vertices.push_back(1);

			// v3
			vertices.push_back(0);
			vertices.push_back(1);
			vertices.push_back(z);

			vertices.push_back(0);
			vertices.push_back(0);
			vertices.push_back(1);
		}
	}

	void create_vbo()
	{
		glGenBuffers(1, &this->vbo);
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

		std::vector<GLfloat> vertices;

		this->push_x_planes(vertices, this->voxelizer->get_side());
		this->push_y_planes(vertices, this->voxelizer->get_side());
		this->push_z_planes(vertices, this->voxelizer->get_side());

		this->vertices_count = vertices.size();
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
	}

	void create_program()
	{
		// Vertex
		Shader vertex_shader(GL_VERTEX_SHADER);
		vertex_shader.source_from_file("resources/shaders/render_voxel_plane.vert.glsl");
		if (!vertex_shader.compile())
		{
			std::cerr << vertex_shader.get_log() << std::endl;
			throw;
		}
		this->program.attach(vertex_shader);

		// Fragment
		Shader fragment_shader(GL_FRAGMENT_SHADER);
		fragment_shader.source_from_file("resources/shaders/render_voxel_plane.frag.glsl");
		if (!fragment_shader.compile())
		{
			std::cerr << fragment_shader.get_log() << std::endl;
			throw;
		}
		this->program.attach(fragment_shader);

		// Program
		if (!this->program.link())
		{
			std::cerr << this->program.get_log() << std::endl;
			throw;
		}
	}

public:
	VoxelizePhase(std::shared_ptr<Voxelizer> voxelizer) : voxelizer(voxelizer)
	{
	}

	void on_enable(PhaseManager* phase_manager)
	{
		std::cout << "================================================================" << std::endl;
		std::cout << "VoxelizePhase" << std::endl;
		std::cout << "================================================================" << std::endl;

		this->create_vbo();
		this->create_program();

		this->voxelizer->voxelize();
		std::cout << "Scene voxelized" << std::endl;

		int width, height;
		glfwGetWindowSize(phase_manager->get_window(), &width, &height);
		glViewport(0, 0, width, height);
		std::cout << "Viewport restored to: " << width << "x" << height << std::endl;

		glDisable(GL_CULL_FACE);
	}

	void on_update(PhaseManager* phase_manager, float delta)
	{
		this->viewer.on_update(phase_manager->get_window(), delta);
	}

	void on_render(PhaseManager* phase_manager)
	{
		this->program.use();

		// Transform
		glm::mat4 transform = glm::mat4(1.0);
		glUniformMatrix4fv(this->program.get_uniform_location("u_transform"), 1, GL_FALSE, glm::value_ptr(transform));

		// Camera
		glm::mat4 camera = this->viewer.get_camera().matrix();
		glUniformMatrix4fv(this->program.get_uniform_location("u_camera"), 1, GL_FALSE, glm::value_ptr(camera));

		// Camera
		glUniform1f(this->program.get_uniform_location("u_voxel_size"), 1 / (GLfloat) this->voxelizer->get_side());

		// Voxel
		GLuint voxel = this->voxelizer->get_voxel();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, voxel);

		// VBO
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

		glDrawArrays(GL_QUADS, 0, this->vertices_count);
	}
};
