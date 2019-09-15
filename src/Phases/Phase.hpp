#pragma once

#include "PhaseManager.hpp"

#include <memory>

class Phase
{
protected:
	std::shared_ptr<PhaseManager> phaseManager;

public:
	Phase(PhaseManager* phaseManager) : phaseManager(phaseManager) {};

	virtual void onEnable() {};
	virtual void onDisable() {};

	virtual void onUpdate(float delta) {};
	virtual void onRender() {};
};
