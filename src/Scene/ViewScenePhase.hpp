#pragma once

#include <memory>

#include "Phase.hpp"
#include "PhaseManager.hpp"

#include "GL/Viewer.hpp"
#include "aiSceneRenderer.hpp"

using namespace mdmc;

class ViewScenePhase : public Phase
{
private:
	GLuint program;

	std::shared_ptr<const aiSceneRenderer> scene_renderer;
	Viewer viewer;

public:
	ViewScenePhase(const aiSceneRenderer& scene_renderer);

	void on_enable(PhaseManager& phase_manager);

	void on_update(PhaseManager& phase_manager, float delta);
	void on_render(PhaseManager& phase_manager);
};
