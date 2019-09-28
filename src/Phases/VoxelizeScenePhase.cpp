#include "VoxelizeScenePhase.hpp"

#include <GLFW/glfw3.h>
#include <iostream>

VoxelizeScenePhase::VoxelizeScenePhase(Scene* scene)
{
	this->scene = scene;
}

void VoxelizeScenePhase::onEnable(PhaseManager* phaseManager)
{
	std::cout << "VOXELIZER > Initializing..." << std::endl;
	voxelizer = std::make_unique<Voxelizer>(Voxelizer());
}

void VoxelizeScenePhase::onRender(PhaseManager* phaseManager)
{
	std::cout << "VOXELIZER > Voxelizing..." << std::endl;
	voxelizer->voxelize(phaseManager->getWindow(), *this->scene, 100);
}
