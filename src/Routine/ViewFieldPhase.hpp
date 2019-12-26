#pragma once

#include <memory>

#include <GL/glew.h>

#include "Phase.hpp"
#include "PhaseManager.hpp"

#include "GL/Viewer.hpp"
#include "Voxelizer/Voxelizer.hpp"

class ViewFieldPhase : public Phase
{
private:
	GLuint program;

	std::shared_ptr<const Voxelizer::Field> field;
	Viewer viewer;

	char camera_mode;

public:
	ViewFieldPhase(std::shared_ptr<const Voxelizer::Field> field);

	void on_enable(PhaseManager& phase_manager);

	void on_update(PhaseManager& phase_manager, float delta);
	void on_render(PhaseManager& phase_manager);
};
