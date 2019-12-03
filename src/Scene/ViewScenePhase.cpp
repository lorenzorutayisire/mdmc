#include "ViewScenePhase.hpp"

#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace mdmc;

ViewScenePhase::ViewScenePhase(const aiSceneRenderer& scene_renderer) : scene_renderer(&scene_renderer)
{
	this->program = glCreateProgram();

	/* Vertex */
	auto vertex = glCreateShader(GL_VERTEX_SHADER);
	{
		std::ifstream file("resources/shaders/visualize.vert.glsl");

		std::string src((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		const GLchar* src_ptr = src.c_str();

		glShaderSource(vertex, 1, &src_ptr, nullptr);
		glCompileShader(vertex);
	}
	glAttachShader(this->program, vertex);

	/* Fragment */
	auto fragment = glCreateShader(GL_FRAGMENT_SHADER);
	{
		std::ifstream file("resources/shaders/visualize.frag.glsl");
		
		std::string src((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		const GLchar* src_ptr = src.c_str();

		glShaderSource(fragment, 1, &src_ptr, nullptr);
		glCompileShader(fragment);
	}
	glAttachShader(this->program, fragment);

	glLinkProgram(this->program);
}

void ViewScenePhase::on_enable(PhaseManager& phase_manager)
{
	glfwSetInputMode(phase_manager.get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void ViewScenePhase::on_update(PhaseManager& phase_manager, float delta)
{
	this->viewer.on_update(phase_manager.get_window(), delta);
}

void ViewScenePhase::on_render(PhaseManager& phase_manager)
{
	glUseProgram(this->program);

	/* Transform */
	auto location = glGetUniformLocation(this->program, "u_transform");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

	/* Camera */
	location = glGetUniformLocation(this->program, "u_camera");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(this->viewer.get_camera().matrix()));

	this->scene_renderer->render();
}
