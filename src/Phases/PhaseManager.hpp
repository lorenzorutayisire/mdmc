#pragma once

#include "Phase.hpp"

#include <memory>

class PhaseManager
{
private:
	std::shared_ptr<Phase> phase;

public:
	PhaseManager();
	~PhaseManager();

	void setPhase(Phase* phase);

	void onUpdate(float delta);
	void onRender();
};
