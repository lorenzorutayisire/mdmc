#include "mc_baker.hpp"

#include <glm/gtc/matrix_transform.hpp>

void mdmc::bake_mc_atlas(GLuint texture, mdmc::mc_atlas const& atlas)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int) atlas.width(), (int) atlas.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, atlas.m_data.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
}

float g_cube_vertices[] = {
	// WEST
	0, 0, 0,	1, 1, // 0
	0, 0, 1,	0, 1, // 1
	0, 1, 1,	0, 0,
	0, 1, 0,	1, 0, // 3

	// EAST
	1, 0, 0,	1, 1,
	1, 1, 0,	1, 0,
	1, 1, 1,	0, 0,
	1, 0, 1,	0, 1,

	// DOWN
	0, 0, 0,	1, 1,
	1, 0, 0,	0, 1, // 2
	1, 0, 1,	0, 0,
	0, 0, 1,	1, 0,

	// UP
	0, 1, 0,	1, 1,
	0, 1, 1,	1, 0,
	1, 1, 1,	0, 0,
	1, 1, 0,	0, 1,

	// BACK
	0, 0, 0,	1, 1,
	0, 1, 0,	1, 0,
	1, 1, 0,	0, 0,
	1, 0, 0,	0, 1,

	// FRONT
	0, 0, 1,	1, 1,
	1, 0, 1,	0, 1,
	1, 1, 1,	0, 0,
	0, 1, 1,	1, 0,
};

size_t bake_mc_model_element_face(
	mdmc::mc_assets const& assets,
	mdmc::mc_model_element_face const& face,
	std::unordered_map<std::string, std::string> const& texture_by_variable,
	glm::mat4 transform,
	std::vector<float>& buffer
)
{
	for (int v_idx = 0; v_idx < 4; v_idx++)
	{
		// Position
		uint32_t idx = (static_cast<unsigned int>(face.m_orientation) * 4 + v_idx) * 5;

		glm::vec3 position;
		position.x = g_cube_vertices[idx];
		position.y = g_cube_vertices[idx + 1];
		position.z = g_cube_vertices[idx + 2];

		position = transform * glm::vec4(position, 1);

		buffer.push_back(position.x);
		buffer.push_back(position.y);
		buffer.push_back(position.z);

		// Tile
		mdmc::mc_atlas::texture texture = face.get_texture(assets, texture_by_variable);
		buffer.push_back((float) texture.m_x);
		buffer.push_back((float) texture.m_y);

		// UV
		unsigned int uv_vertex = (v_idx + face.m_rotation / 90) % 4;
		idx = (static_cast<unsigned int>(face.m_orientation) * 4 + uv_vertex) * 5;

		glm::vec2 uv;
		uv.x = g_cube_vertices[idx + 3];
		uv.y = g_cube_vertices[idx + 4];

		uv *= face.m_to_uv - face.m_from_uv;
		uv += face.m_from_uv;

		buffer.push_back(uv.x);
		buffer.push_back(uv.y);

		// Tint
		buffer.push_back(face.m_tint_index);
	}
	return 4;
}

size_t bake_mc_model_element(
	mdmc::mc_assets const& assets,
	mdmc::mc_model_element const& element,
	std::unordered_map<std::string, std::string> const& texture_by_variable,
	glm::mat4 transform,
	std::vector<float>& buffer
)
{
	if (element.m_rotation.m_angle != 0)
	{
		transform = glm::translate(transform, element.m_rotation.m_origin);
		transform = glm::rotate(
			transform,
			glm::radians(element.m_rotation.m_angle),
			glm::vec3(
				element.m_rotation.m_axis == 'x' ? 1 : 0,
				element.m_rotation.m_axis == 'y' ? 1 : 0,
				element.m_rotation.m_axis == 'z' ? 1 : 0
			)
		);
		transform = glm::translate(transform, -element.m_rotation.m_origin);
	}

	transform = glm::translate(transform, glm::vec3(element.m_from));
	transform = glm::scale(transform, glm::vec3(element.m_to - element.m_from));

	size_t vertices_count = 0;

	// Some models may only be implementations and their faces are defined in parent models.
	if (!element.m_face_by_position.empty())
	{
		for (int i = 0; i < static_cast<int>(mdmc::mc_model_element_face_orientation::Count); i++)
		{
			auto orientation = static_cast<mdmc::mc_model_element_face_orientation>(i);
			auto face = element.m_face_by_position.at(orientation);
			vertices_count += bake_mc_model_element_face(assets, face, texture_by_variable, transform, buffer);
		}
	}
	return vertices_count;
}

size_t bake_mc_model(
	mdmc::mc_assets const& assets,
	mdmc::mc_model const& model,
	glm::mat4 transform,
	std::vector<float>& buffer
)
{
	size_t vertices_count = 0;

	mdmc::mc_model const* current_model = &model;
	std::unordered_map<std::string, std::string> texture_by_variable{};

	while (current_model)
	{
		texture_by_variable.insert(current_model->m_texture_by_variable.begin(), current_model->m_texture_by_variable.end());
		if (!current_model->m_elements.empty())
		{
			for (auto& element : current_model->m_elements) {
				vertices_count += bake_mc_model_element(assets, element, texture_by_variable, transform, buffer);
			}
			return vertices_count;
		}

		if (current_model->m_parent_model.empty()) {
			current_model = nullptr;
		} else {
			current_model = &assets.m_model_by_name.find(current_model->m_parent_model)->second;
		}
	}

	return 0;
}

size_t mdmc::bake_mc_block_state_variant(
	mdmc::mc_assets const& assets,
	mdmc::mc_block_state_variant const& block_state_variant,
	glm::mat4 transform,
	std::vector<float>& buffer
)
{
	transform = glm::translate(transform, glm::vec3(8, 8, 8));
	transform = glm::rotate(transform, glm::radians<float>((float) block_state_variant.m_x), glm::vec3(1, 0, 0));
	transform = glm::rotate(transform, glm::radians<float>((float) block_state_variant.m_y), glm::vec3(0, 1, 0));
	transform = glm::translate(transform, -glm::vec3(8, 8, 8));

	mdmc::mc_model const& model = assets.m_model_by_name.at(block_state_variant.m_model);
	size_t v_count = bake_mc_model(assets, model, transform, buffer);
	return v_count;
}

void mdmc::define_block_vertex_layout(GLuint vao)
{
	glBindVertexArray(vao);

	GLuint loc;
	size_t shift = 0;

	// Position
	loc = 0;
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, MDMC_VERTEX_SIZE * sizeof(GLfloat), (void*) shift);
	shift += 3 * sizeof(GLfloat);

	// Tile
	loc = 1;
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, MDMC_VERTEX_SIZE * sizeof(GLfloat), (void*) shift);
	shift += 2 * sizeof(GLfloat);

	// Uv
	loc = 2;
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, MDMC_VERTEX_SIZE * sizeof(GLfloat), (void*) shift);
	shift += 2 * sizeof(GLfloat);

	// Tint color
	loc = 3;
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 1, GL_FLOAT, GL_FALSE, MDMC_VERTEX_SIZE * sizeof(GLfloat), (void*) shift);
	//shift += 1 * sizeof(GLfloat);
}
