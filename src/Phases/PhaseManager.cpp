#include "PhaseManager.hpp"

PhaseManager::PhaseManager() : phase(nullptr)
{
}

PhaseManager::~PhaseManager()
{
}

void PhaseManager::setPhase(Phase* phase)
{
	if (this->phase)
	{
		this->phase->onDisable();
	}
	this->phase = std::shared_ptr<Phase>(phase);
	if (this->phase)
	{
		this->phase->onEnable();
	}
}

void PhaseManager::onUpdate(float delta)
{
	if (this->phase)
	{
		this->phase->onUpdate(delta);
	}
}

void PhaseManager::onRender()
{
	if (this->phase)
	{
		this->phase->onRender();
	}
}
