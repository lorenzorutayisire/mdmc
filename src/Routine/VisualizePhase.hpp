#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Phase.hpp"
#include "PhaseManager.hpp"

#include "GL/Scene.hpp"

#include "GL/Shader.hpp"
#include "GL/Program.hpp"

#include "GL/Camera.hpp"

class VisualizePhase : public Phase
{
private:
	Scene* scene;
	Program program;

	tdogl::Camera camera;

	glm::mat4 x_ortho_proj;
	glm::mat4 y_ortho_proj;
	glm::mat4 z_ortho_proj;

	char camera_selection;

	void create_program()
	{
		Shader vertex_shader(GL_VERTEX_SHADER);
		vertex_shader.source_from_file("resources/shaders/visualize.vert.glsl");
		vertex_shader.compile();
		this->program.attach(vertex_shader);

		Shader fragment_shader(GL_FRAGMENT_SHADER);
		fragment_shader.source_from_file("resources/shaders/visualize.frag.glsl");
		fragment_shader.compile();
		this->program.attach(vertex_shader);

		this->program.link();
	}

public:
	VisualizePhase(Scene* scene)
	{
		this->scene = scene;

		glm::mat4 ortho = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 2.0f - 1.0f, 3.0f);
		this->x_ortho_proj = ortho * glm::lookAt(glm::vec3(2, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		this->y_ortho_proj = ortho * glm::lookAt(glm::vec3(0, 2, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));
		this->z_ortho_proj = ortho * glm::lookAt(glm::vec3(0, 0, 2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		this->camera_selection = 'c';
	}

	void on_enable(PhaseManager* phase_manager)
	{
		std::cout << "================================================================" << std::endl;
		std::cout << "Visualizing scene" << std::endl;
		std::cout << "================================================================" << std::endl;

		this->create_program();
		glfwSetInputMode(phase_manager->get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	void set_camera_selection(char camera_selection)
	{
		if (this->camera_selection != camera_selection)
		{
			this->camera_selection = camera_selection;
			std::cout << "Camera changed to: " << camera_selection << std::endl;
		}
	}

	void on_update(PhaseManager* phase_manager, float delta)
	{
		if (glfwGetKey(phase_manager->get_window(), GLFW_KEY_X) == GLFW_PRESS) { this->set_camera_selection('x'); }
		if (glfwGetKey(phase_manager->get_window(), GLFW_KEY_Y) == GLFW_PRESS) { this->set_camera_selection('y'); }
		if (glfwGetKey(phase_manager->get_window(), GLFW_KEY_Z) == GLFW_PRESS) { this->set_camera_selection('z'); }
		if (glfwGetKey(phase_manager->get_window(), GLFW_KEY_C) == GLFW_PRESS) { this->set_camera_selection('c'); }
	}

	void on_render(PhaseManager* phase_manager)
	{
		this->program.use();

		switch (this->camera_selection)
		{
		case 'x':
			glUniformMatrix4fv(this->program.get_uniform_location("u_camera"), 1, GL_FALSE, glm::value_ptr(this->x_ortho_proj));
			break;
		case 'y':
			glUniformMatrix4fv(this->program.get_uniform_location("u_camera"), 1, GL_FALSE, glm::value_ptr(this->y_ortho_proj));
			break;
		case 'z':
			glUniformMatrix4fv(this->program.get_uniform_location("u_camera"), 1, GL_FALSE, glm::value_ptr(this->z_ortho_proj));
			break;
		case 'c':
			glUniformMatrix4fv(this->program.get_uniform_location("u_camera"), 1, GL_FALSE, glm::value_ptr(this->camera.matrix()));
			break;
		}

		this->scene->render();
	}
};
