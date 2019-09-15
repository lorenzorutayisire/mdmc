#pragma once

#include "Phase.hpp"
#include "../Scene/Scene.hpp"

#include <memory>

class VisualizeScenePhase : public Phase
{
private:
	std::shared_ptr<Scene> scene;

public:
	VisualizeScenePhase(PhaseManager* phaseManager, Scene* scene);

	void onEnable();
	void onDisable();

	void onUpdate(float delta);
	void onRender();
};
