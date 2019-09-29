#pragma once

#include "PhaseManager.hpp"
#include "Phase.hpp"

#include "../Scene/Scene.hpp"
#include "../Voxelizer.hpp"

#include "../Program.hpp"

#include "../Camera.hpp"

class VoxelizeScenePhase : public Phase
{
private:
	Scene* scene;
	std::unique_ptr<Voxelizer> voxelizer;

	tdogl::Camera camera;
	Program program;

	GLuint vbo;
	GLuint vertices_count;

	double lastCursorX, lastCursorY;

public:
	VoxelizeScenePhase(Scene* scene);

	void onEnable(PhaseManager* phaseManager);
	void onUpdate(PhaseManager* phaseManager, float delta);
	void onRender(PhaseManager* phaseManager);
};
