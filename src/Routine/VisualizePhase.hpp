#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <memory>

#include "Phase.hpp"
#include "PhaseManager.hpp"

#include "GL/Scene.hpp"

#include "GL/Shader.hpp"
#include "GL/Program.hpp"

#include "GL/Viewer.hpp"

#include "Voxelizer.hpp"

class VisualizePhase : public Phase
{
private:
	std::shared_ptr<Voxelizer> voxelizer;
	Program program;
	Viewer viewer;

	char camera_mode;

	void create_program()
	{
		Shader vertex_shader(GL_VERTEX_SHADER);
		vertex_shader.source_from_file("resources/shaders/visualize.vert.glsl");
		if (!vertex_shader.compile())
		{
			std::cerr << vertex_shader.get_log() << std::endl;
			throw;
		}
		this->program.attach(vertex_shader);

		Shader fragment_shader(GL_FRAGMENT_SHADER);
		fragment_shader.source_from_file("resources/shaders/visualize.frag.glsl");
		if (!fragment_shader.compile())
		{
			std::cerr << fragment_shader.get_log() << std::endl;
			throw;
		}
		this->program.attach(fragment_shader);

		if (!this->program.link())
		{
			std::cerr << this->program.get_log() << std::endl;
			throw;
		}
	}

public:
	VisualizePhase(Scene* scene)
	{
		this->voxelizer = std::make_shared<Voxelizer>(Voxelizer(*scene, 256));

		glm::mat4 transform = glm::mat4(1.0);
		transform = glm::scale(transform, glm::vec3(1 / scene->get_size().y));
		transform = glm::translate(transform, -scene->get_min_vertex());
		glUniformMatrix4fv(this->program.get_uniform_location("u_transform"), 1, GL_FALSE, glm::value_ptr(transform));


		this->camera_mode = 'c';
	}

	void on_enable(PhaseManager* phase_manager)
	{
		std::cout << "================================================================" << std::endl;
		std::cout << "Visualizing scene" << std::endl;
		std::cout << "================================================================" << std::endl;

		this->create_program();
		glfwSetInputMode(phase_manager->get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	void set_camera_mode(char camera_mode)
	{
		if (this->camera_mode != camera_mode)
		{
			this->camera_mode = camera_mode;
			std::cout << "camera_mode: " << camera_mode << std::endl;
		}
	}

	void on_update(PhaseManager* phase_manager, float delta)
	{
		if (camera_mode == 'c')
		{
			this->viewer.on_update(phase_manager->get_window(), delta);
		}

		if (glfwGetKey(phase_manager->get_window(), GLFW_KEY_X) == GLFW_PRESS) { this->set_camera_mode('x'); }
		if (glfwGetKey(phase_manager->get_window(), GLFW_KEY_Y) == GLFW_PRESS) { this->set_camera_mode('y'); }
		if (glfwGetKey(phase_manager->get_window(), GLFW_KEY_Z) == GLFW_PRESS) { this->set_camera_mode('z'); }
		if (glfwGetKey(phase_manager->get_window(), GLFW_KEY_C) == GLFW_PRESS) { this->set_camera_mode('c'); }
	}

	void on_render(PhaseManager* phase_manager)
	{
		this->program.use();

		// Transform
		glUniformMatrix4fv(this->program.get_uniform_location("u_transform"), 1, GL_FALSE, glm::value_ptr(this->voxelizer->get_transform()));

		// Camera
		switch (this->camera_mode)
		{
		case 'x':
			glUniformMatrix4fv(this->program.get_uniform_location("u_camera"), 1, GL_FALSE, glm::value_ptr(this->voxelizer->get_x_ortho_projection()));
			break;
		case 'y':
			glUniformMatrix4fv(this->program.get_uniform_location("u_camera"), 1, GL_FALSE, glm::value_ptr(this->voxelizer->get_y_ortho_projection()));
			break;
		case 'z':
			glUniformMatrix4fv(this->program.get_uniform_location("u_camera"), 1, GL_FALSE, glm::value_ptr(this->voxelizer->get_z_ortho_projection()));
			break;
		case 'c':
			glUniformMatrix4fv(this->program.get_uniform_location("u_camera"), 1, GL_FALSE, glm::value_ptr(this->viewer.get_camera().matrix()));
			break;
		}

		this->voxelizer->get_scene()->render();
	}
};
