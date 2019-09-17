#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <memory>

class PhaseManager;

class Phase
{
public:
	virtual void onEnable(PhaseManager* phaseManager) {};
	virtual void onDisable(PhaseManager* phaseManager) {};
	virtual void onUpdate(PhaseManager* phaseManager, float delta) {};
	virtual void onRender(PhaseManager* phaseManager) {};
};
