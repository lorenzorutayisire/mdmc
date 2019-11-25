#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <memory>

class PhaseManager;

class Phase
{
public:
	virtual void on_enable(PhaseManager* phase_manager) {};
	virtual void on_disable(PhaseManager* phase_manager) {};

	virtual void on_update(PhaseManager* phase_manager, float delta) {};

	virtual void on_render(PhaseManager* phase_manager) {};
	virtual void on_render_ui(PhaseManager* phase_manager) {};
};
