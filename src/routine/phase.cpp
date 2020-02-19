#include "phase.hpp"

Stage::Stage(GLFWwindow* window) :
	window(window)
{}

std::shared_ptr<Phase> Stage::get_phase() const
{
	return phase;
}

void Stage::set_phase(std::shared_ptr<Phase> phase)
{
	if (this->phase != nullptr)
		this->phase->disable(*this);

	this->phase = phase;

	if (this->phase != nullptr)
		this->phase->enable(*this);
}

void Stage::update(float delta)
{
	if (this->phase)
		this->phase->update(*this, delta);
}

void Stage::render()
{
	if (this->phase)
		this->phase->render(*this);
}
