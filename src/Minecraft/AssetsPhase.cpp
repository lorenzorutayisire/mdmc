#include "AssetsPhase.hpp"

#include <imgui.h>

using namespace mdmc;

GLfloat outline_cube_vertices[] = {
	/* Position | UV | Texture |    Tint  */
	0,  0,  0,	 0, 0,    0,     0, 0, 0, 0,
	0,  0,  16,  0, 0,    0,     0, 0, 0, 0,
	0,  16, 16,  0, 0,    0,     0, 0, 0, 0,
	16, 16, 16,  0, 0,    0,     0, 0, 0, 0,
	16, 16, 0,   0, 0,    0,     0, 0, 0, 0,
	0,  16, 0,   0, 0,    0,     0, 0, 0, 0,
	0,  0,  0,   0, 0,    0,     0, 0, 0, 0,
	16, 0,  0,   0, 0,    0,     0, 0, 0, 0,
	16, 0,  16,  0, 0,    0,     0, 0, 0, 0,
	0,  0,  16,  0, 0,    0,     0, 0, 0, 0,
	0,  16, 16,  0, 0,    0,     0, 0, 0, 0,
	0,  16, 0,   0, 0,    0,     0, 0, 0, 0,
	16, 16, 0,   0, 0,    0,     0, 0, 0, 0,
	16, 0,  0,   0, 0,    0,     0, 0, 0, 0,
	16, 0,  16,  0, 0,    0,     0, 0, 0, 0,
	16, 16, 16,  0, 0,    0,     0, 0, 0, 0
};

GLuint outline_cube_vbo;

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

	glGenBuffers(1, &outline_cube_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, outline_cube_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(outline_cube_vertices), outline_cube_vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void AssetsPhase::set_selected_block_id(size_t selected_block_id)
{
	auto t0 = glfwGetTime();

	if (t0 - last_selection_change >= .25)
	{
		this->selected_block_id = selected_block_id > this->assets->get_blocks().size() ? (this->assets->get_blocks().size() - 1) : selected_block_id;

		last_selection_change = t0;

		std::cout << "Selected block ID: " << this->selected_block_id << std::endl;
	}
}

void AssetsPhase::on_update(PhaseManager* phase_manager, float delta)
{
	this->viewer.on_update(phase_manager->get_window(), delta);

	if (glfwGetKey(phase_manager->get_window(), GLFW_KEY_LEFT) == GLFW_PRESS)
		this->set_selected_block_id(selected_block_id - 1);

	if (glfwGetKey(phase_manager->get_window(), GLFW_KEY_RIGHT) == GLFW_PRESS)
		this->set_selected_block_id(selected_block_id + 1);
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
	glBindTexture(GL_TEXTURE_2D_ARRAY, this->assets->get_atlas());

	/* VBO */
	glBindBuffer(GL_ARRAY_BUFFER, this->assets->get_vbo());

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

	Block block = this->assets->get_block(this->selected_block_id);
	glDrawArrays(GL_QUADS, block.vertices_offset, block.vertices_count);

	/* Outline cube VBO */

	glBindBuffer(GL_ARRAY_BUFFER, outline_cube_vbo);
	
	// \_ Position
	location = this->program.get_attrib_location("position");
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

	glDrawArrays(GL_LINE_STRIP, 0, sizeof(outline_cube_vertices) / 10);
}


void AssetsPhase::on_render_ui(PhaseManager* phase_manager)
{
	ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background

	ImGui::Begin("Block ID", NULL, (
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoNav
	));

	auto& block = this->assets->get_block(this->selected_block_id);

	ImGui::Text(block.name.c_str());

	ImGui::End();
}


