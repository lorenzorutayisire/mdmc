#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <memory>

#include "Phase.hpp"

class PhaseManager
{
private:
	GLFWwindow* window;

	std::shared_ptr<Phase> phase;

public:
	PhaseManager(GLFWwindow* window) : window(window) {}

	GLFWwindow* get_window() { return this->window; }

	void set_phase(Phase* phase)
	{
		if (this->phase)
		{
			this->phase->on_disable(this);
		}
		this->phase = std::shared_ptr<Phase>(phase);
		if (this->phase)
		{
			this->phase->on_enable(this);
		}
	}

	void on_update(float delta)
	{
		if (this->phase)
		{
			this->phase->on_update(this, delta);
		}
	}

	void on_render()
	{
		if (this->phase)
		{
			this->phase->on_render(this);
		}
	}
};
