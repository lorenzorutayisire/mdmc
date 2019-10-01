#pragma once

#include <string>

#include "Phase.hpp"

class LoadScenePhase : public Phase
{
private:
	std::string path;

public:
	LoadScenePhase(const char* path);

	void onEnable(PhaseManager* phaseManager);

	void onRender(PhaseManager* phaseManager);
};
