#pragma once

#include "Phase.hpp"

#include "../Scene/Scene.hpp"
#include "../Voxelizer.hpp"

class VoxelizeScenePhase : public Phase
{
private:
	Scene* scene;
	std::unique_ptr<Voxelizer> voxelizer;

public:
	VoxelizeScenePhase(Scene* scene);

	void onEnable(PhaseManager* phaseManager);
	void onRender(PhaseManager* phaseManager);
};
