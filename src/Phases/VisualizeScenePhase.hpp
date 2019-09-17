#pragma once

#include <GL/glew.h>

#include <memory>

#include "PhaseManager.hpp"
#include "Phase.hpp"
#include "../Scene/Scene.hpp"
#include "../Camera.hpp"

class VisualizeScenePhase : public Phase
{
private:
	double lastCursorX, lastCursorY;

	std::shared_ptr<Scene> scene;

	GLuint program;
	tdogl::Camera camera;

public:
	VisualizeScenePhase(Scene* scene);

	void onEnable(PhaseManager* phaseManager);

	void onDisable(PhaseManager* phaseManager);

	void onUpdate(PhaseManager* phaseManager, float delta);

	void onRender(PhaseManager* phaseManager);
};
