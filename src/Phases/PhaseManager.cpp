#include "PhaseManager.hpp"
#include "Phase.hpp"

PhaseManager::PhaseManager(GLFWwindow* window) :
	window(window),
	phase(nullptr)
{
}

PhaseManager::~PhaseManager()
{
}

GLFWwindow* PhaseManager::getWindow()
{
	return window;
}

void PhaseManager::setPhase(Phase* phase)
{
	if (this->phase)
	{
		this->phase->onDisable(this);
	}
	this->phase = std::shared_ptr<Phase>(phase);
	if (this->phase)
	{
		this->phase->onEnable(this);
	}
}

void PhaseManager::onUpdate(float delta)
{
	if (this->phase)
	{
		this->phase->onUpdate(this, delta);
	}
}

void PhaseManager::onRender()
{
	if (this->phase)
	{
		this->phase->onRender(this);
	}
}
