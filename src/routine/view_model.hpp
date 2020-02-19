#pragma once

#include "phase.hpp"
#include "field.hpp"
#include "gl.hpp"

class ViewModel : public Phase
{
private:
	Program program;
	std::shader_ptr<const Field> field;

public:
	ViewModel(const std::shared_ptr<const Field>& field)
	{
		this->field = field;

		Shader vert(GL_VERTEX_SHADER);

		vert.source_from_file("resources/shaders/render_model.vert");
		vert.compile();

		this->program.attach_shader(vert);

		Shader frag(GL_FRAGMENT_SHADER);

		frag.source_from_file("resources/shaders/render_model.frag");
		frag.compile();

		program.attach_shader(frag);
	}

	void update(Stage& stage, float delta) override
	{

	}

	void render(Stage& stage) override
	{
		this->program.use();

		this->field->render();

		this->program.unuse();
	}
};
