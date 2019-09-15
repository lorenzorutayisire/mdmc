#include "VisualizeScenePhase.hpp"

VisualizeScenePhase::VisualizeScenePhase(PhaseManager *phaseManager, Scene* scene) : Phase(phaseManager), scene(scene)
{
}

void VisualizeScenePhase::onEnable()
{
}

void VisualizeScenePhase::onDisable()
{
}

void VisualizeScenePhase::onUpdate(float delta)
{
}

void VisualizeScenePhase::onRender()
{
	this->scene->render();
}
