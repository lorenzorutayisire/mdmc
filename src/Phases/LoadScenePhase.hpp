#pragma once

#include "Phase.hpp"

#include <string>

class LoadScenePhase : public Phase
{
private:
	std::string path;

public:
	LoadScenePhase(PhaseManager* phaseManager, const char* path);

	void onEnable();

	void onRender();
};
