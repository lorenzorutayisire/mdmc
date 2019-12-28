#include "ViewFieldPhase.hpp"

#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Minecraft/Assets.hpp"


// ================================================================================================
// Guideline Cube
// ================================================================================================

GLfloat guideline_cube_vertices[] = {
	0, 0, 0,
	0, 0, 1,
	0, 1, 1,
	1, 1, 1,
	1, 1, 0,
	0, 1, 0,
	0, 0, 0,
	1, 0, 0,
	1, 0, 1,
	0, 0, 1,
	0, 1, 1,
	0, 1, 0,
	1, 1, 0,
	1, 0, 0,
	1, 0, 1,
	1, 1, 1
};

GLuint guideline_cube_vbo;

void guideline_cube_store()
{
	glGenBuffers(1, &guideline_cube_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, guideline_cube_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(guideline_cube_vertices), guideline_cube_vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void guideline_cube_draw()
{
	glBindBuffer(GL_ARRAY_BUFFER, guideline_cube_vbo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	glDrawArrays(GL_LINE_STRIP, 0, sizeof(guideline_cube_vertices) / 3);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// ================================================================================================
// ViewFieldPhase
// ================================================================================================

ViewFieldPhase::ViewFieldPhase(std::shared_ptr<const Voxelizer::Field> field) : field(field), camera_mode('c')
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


	guideline_cube_store();
}

void ViewFieldPhase::on_enable(PhaseManager& phase_manager)
{
	glfwSetInputMode(phase_manager.get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void ViewFieldPhase::on_update(PhaseManager& phase_manager, float delta)
{
	if (this->camera_mode == 'c')
		this->viewer.on_update(phase_manager.get_window(), delta);

	if (glfwGetKey(phase_manager.get_window(), GLFW_KEY_X) == GLFW_PRESS)
		this->camera_mode = 'x';

	if (glfwGetKey(phase_manager.get_window(), GLFW_KEY_Y) == GLFW_PRESS)
		this->camera_mode = 'y';

	if (glfwGetKey(phase_manager.get_window(), GLFW_KEY_Z) == GLFW_PRESS)
		this->camera_mode = 'z';

	if (glfwGetKey(phase_manager.get_window(), GLFW_KEY_C) == GLFW_PRESS)
		this->camera_mode = 'c';

	if (glfwGetKey(phase_manager.get_window(), GLFW_KEY_ENTER) == GLFW_PRESS)
	{
		auto assets = std::make_shared<mdmc::Minecraft::Assets>("tmp/mc_assets", "1.14");
		assets->load();

		this->field = assets;
	}
}

void ViewFieldPhase::on_render(PhaseManager& phase_manager)
{
	glUseProgram(this->program);

	// Camera
	GLint location;
	location = glGetUniformLocation(this->program, "u_camera");
	
	switch (camera_mode)
	{
	case 'x':
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(this->field->x_proj()));
		break;
	case 'y':
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(this->field->y_proj()));
		break;
	case 'z':
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(this->field->z_proj()));
		break;
	case 'c':
	default:
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(this->viewer.get_camera().matrix() * this->field->transform()));
		break;
	}

	this->field->render();

	// Overall - guideline
	guideline_cube_draw();

	if (camera_mode == 'c')
	{
		glm::mat4 cam_x_pos = this->viewer.get_camera().matrix() * this->field->transform();

		// Min - guideline
		glm::mat4 min_t = glm::mat4(1.0f);
		min_t = glm::translate(min_t, this->field->min);
		min_t = glm::scale(min_t, glm::vec3(0.1));
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(cam_x_pos * min_t));

		guideline_cube_draw();

		// Max - guideline
		glm::mat4 max_t = glm::mat4(1.0f);
		max_t = glm::translate(max_t, this->field->max);
		max_t = glm::scale(max_t, glm::vec3(0.1));
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(cam_x_pos * max_t));

		guideline_cube_draw();
	}

}
