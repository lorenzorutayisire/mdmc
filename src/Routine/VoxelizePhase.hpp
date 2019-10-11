#pragma once

#include <iostream>

#include "GL/Program.hpp"

#include "Phase.hpp"
#include "Voxelizer.hpp"

#include "Minecraft/TextureAsset.hpp"

class VoxelizePhase : public Phase
{
private:
	GLuint vbo;
	GLuint vertices_count;

	Program program;

	Viewer viewer;

	std::shared_ptr<Voxelizer> voxelizer;
	TextureAsset texture_asset;

	uint8_t color_mode;

	GLuint minecraft_blocks;

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
		vertex_shader.source_from_file("resources/shaders/render_voxel.vert.glsl");
		if (!vertex_shader.compile())
		{
			std::cerr << vertex_shader.get_log() << std::endl;
			throw;
		}
		this->program.attach(vertex_shader);

		// Fragment
		Shader fragment_shader(GL_FRAGMENT_SHADER);
		fragment_shader.source_from_file("resources/shaders/render_voxel.frag.glsl");
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
	VoxelizePhase(std::shared_ptr<Voxelizer> voxelizer) :
		voxelizer(voxelizer)
	{
	}

	void compute_minecraft_blocks()
	{
		Program compute_program;

		Shader compute_shader(GL_COMPUTE_SHADER);
		compute_shader.source_from_file("resources/shaders/find_minecraft_nearest_block.comp.glsl");
		compute_shader.compile();

		compute_program.attach(compute_shader);
		compute_program.link();

		compute_program.use();

		// voxel
		// (x, y, z) -> (scene.r, scene.g, scene.b, scene.a)

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, this->voxelizer->get_voxel());
		glBindImageTexture(0, this->voxelizer->get_voxel(), 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);

		// textures_averages
		// block_id -> (avg.r, avg.g, avg.b, avg.a)

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D_ARRAY, this->texture_asset.get_textures_averages());
		glBindImageTexture(1, this->texture_asset.get_textures_averages(), 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);

		// minecraft_blocks
		// (x, y, z) -> block_id

		glGenTextures(1, &this->minecraft_blocks);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_3D, this->minecraft_blocks);

		glTexImage3D(
			GL_TEXTURE_3D,
			0,
			GL_R32UI,
			this->voxelizer->get_side(),
			this->voxelizer->get_side(),
			this->voxelizer->get_side(),
			0,
			GL_R,
			GL_UNSIGNED_INT,
			NULL
		);

		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glBindImageTexture(2, this->minecraft_blocks, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32UI);

		glDispatchCompute(
			this->voxelizer->get_side(),
			this->voxelizer->get_side(),
			this->voxelizer->get_side()
		);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}

	void on_enable(PhaseManager* phase_manager)
	{
		std::cout << "================================================================" << std::endl;
		std::cout << "VoxelizePhase" << std::endl;
		std::cout << "================================================================" << std::endl;

		this->create_vbo();
		this->create_program();

		// voxelize scene
		this->voxelizer->voxelize();
		std::cout << "Scene voxelized" << std::endl;

		// texture asset
		std::ifstream texture_asset_file("resources/minecraft_assets/1.8.8.bin", std::ios_base::binary);
		if (errno)
		{
			std::cerr << strerror(errno) << std::endl;
			throw;
		}
		this->texture_asset.load(texture_asset_file);
		std::cout << "Minecraft texture asset loaded" << std::endl;

		// compute minecraft blocks
		this->compute_minecraft_blocks();
		std::cout << "Minecraft blocks associated" << std::endl;

		int width, height;
		glfwGetWindowSize(phase_manager->get_window(), &width, &height);
		glViewport(0, 0, width, height);
		std::cout << "Viewport restored to: " << width << "x" << height << std::endl;

		glDisable(GL_CULL_FACE);
	}

	void set_color_mode(uint8_t color_mode)
	{
		if (this->color_mode != color_mode)
		{
			this->color_mode = color_mode;
			std::cout << "Color mode: " << this->color_mode << std::endl;
		}
	}

	void on_update(PhaseManager* phase_manager, float delta)
	{
		this->viewer.on_update(phase_manager->get_window(), delta);

		if (glfwGetKey(phase_manager->get_window(), GLFW_KEY_1) == GLFW_PRESS) { this->set_color_mode(0); }
		if (glfwGetKey(phase_manager->get_window(), GLFW_KEY_2) == GLFW_PRESS) { this->set_color_mode(1); }
		if (glfwGetKey(phase_manager->get_window(), GLFW_KEY_3) == GLFW_PRESS) { this->set_color_mode(2); }
	}

	void on_render(PhaseManager* phase_manager)
	{
		this->program.use();

		/////////////////////////////////////////////////////////////////////////////

		// Transform
		glm::mat4 transform = glm::mat4(1.0);
		glUniformMatrix4fv(this->program.get_uniform_location("u_transform"), 1, GL_FALSE, glm::value_ptr(transform));

		// Camera
		glm::mat4 camera = this->viewer.get_camera().matrix();
		glUniformMatrix4fv(this->program.get_uniform_location("u_camera"), 1, GL_FALSE, glm::value_ptr(camera));

		// voxel_size
		glUniform1f(this->program.get_uniform_location("u_voxel_size"), 1 / (GLfloat) this->voxelizer->get_side());

		// camera_mode
		glUniform1ui(this->program.get_uniform_location("u_color_mode"), this->color_mode);

		/////////////////////////////////////////////////////////////////////////////

		// voxel
		GLuint voxel = this->voxelizer->get_voxel();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, voxel);

		// minecraft_blocks
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, this->minecraft_blocks);
		glBindImageTexture(1, this->minecraft_blocks, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32UI);

		// minecraft_avg
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D_ARRAY, this->texture_asset.get_textures_averages());

		// minecraft_textures
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D_ARRAY, this->texture_asset.get_textures());

		/////////////////////////////////////////////////////////////////////////////

		// VBO
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

		glDrawArrays(GL_QUADS, 0, this->vertices_count);
	}
};
