#include "AssetsPhase.hpp"

using namespace mdmc;

AssetsPhase::AssetsPhase(const Assets& assets) : assets(&assets), selected_block_id(0)
{
	std::cout << "================================================================" << std::endl;
	std::cout << "VisualizingPhase" << std::endl;
	std::cout << "================================================================" << std::endl;

	/* Vertex */
	Shader v_shader(GL_VERTEX_SHADER);
	v_shader.source_from_file("resources/shaders/assets.vert.glsl");
	if (!v_shader.compile())
	{
		std::cerr << v_shader.get_log() << std::endl;
		throw;
	}
	this->program.attach(v_shader);

	/* Fragment */
	Shader f_shader(GL_FRAGMENT_SHADER);
	f_shader.source_from_file("resources/shaders/assets.frag.glsl");
	if (!f_shader.compile())
	{
		std::cerr << f_shader.get_log() << std::endl;
		throw;
	}
	this->program.attach(f_shader);

	if (!this->program.link())
	{
		std::cerr << this->program.get_log() << std::endl;
		throw;
	}
}

void AssetsPhase::on_update(PhaseManager* phase_manager, float delta)
{
	this->viewer.on_update(phase_manager->get_window(), delta);

	if (glfwGetKey(phase_manager->get_window(), GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		if (selected_block_id == 0)
			selected_block_id = this->assets->blocks_by_id.size() - 1;
		else
			selected_block_id--;

		std::cout << "Selected block ID: " << selected_block_id << std::endl;
	}

	if (glfwGetKey(phase_manager->get_window(), GLFW_KEY_RIGHT) == GLFW_PRESS && !just_selected)
	{
		selected_block_id = (selected_block_id + 1) % this->assets->blocks_by_id.size();
		std::cout << "Selected block ID: " << selected_block_id << std::endl;
		just_selected = true;
	}

	if (glfwGetKey(phase_manager->get_window(), GLFW_KEY_LEFT) == GLFW_RELEASE || glfwGetKey(phase_manager->get_window(), GLFW_KEY_RIGHT) == GLFW_RELEASE)
	{
		just_selected = false;
	}
}

void AssetsPhase::on_render(PhaseManager* phase_manager)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.5, 0, 0, 0);

	this->program.use();

	/* Transform */
	glUniformMatrix4fv(this->program.get_uniform_location("u_transform"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

	/* Camera */
	glUniformMatrix4fv(this->program.get_uniform_location("u_camera"), 1, GL_FALSE, glm::value_ptr(this->viewer.get_camera().matrix()));

	/* Textures */
	glBindTexture(GL_TEXTURE_2D_ARRAY, this->assets->textures);

	/* VBO */
	glBindBuffer(GL_ARRAY_BUFFER, this->assets->vbo);

	// \_ Position
	auto location = this->program.get_attrib_location("position");
	glEnableVertexAttribArray(location);
	glVertexAttribPointer(location, 3, GL_FLOAT, false, (3 + 2 + 1 + 4) * sizeof(GLfloat), (void*)(0 * sizeof(GLfloat)));

	// \_ UV
	location = this->program.get_attrib_location("uv");
	glEnableVertexAttribArray(location);
	glVertexAttribPointer(location, 2, GL_FLOAT, false, (3 + 2 + 1 + 4) * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	// \_ Texture Id
	location = this->program.get_attrib_location("texture_id");
	glEnableVertexAttribArray(location);
	glVertexAttribPointer(location, 1, GL_FLOAT, false, (3 + 2 + 1 + 4) * sizeof(GLfloat), (void*)((3 + 2) * sizeof(GLfloat)));

	// \_ Tint
	location = this->program.get_attrib_location("tint");
	glEnableVertexAttribArray(location);
	glVertexAttribPointer(location, 4, GL_FLOAT, false, (3 + 2 + 1 + 4) * sizeof(GLfloat), (void*)((3 + 2 + 1) * sizeof(GLfloat)));

	Block block = this->assets->blocks_by_id[this->selected_block_id];
	glDrawArrays(GL_QUADS, block.start_vertex_offset, block.vertices_count);
}


