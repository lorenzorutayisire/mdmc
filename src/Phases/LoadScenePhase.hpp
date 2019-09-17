#pragma once

#include "Phase.hpp"

#include <string>

class LoadScenePhase : public Phase
{
private:
	std::string path;

public:
	LoadScenePhase(const char* path);

	void onEnable(PhaseManager* phaseManager);

	void onRender(PhaseManager* phaseManager);
};
