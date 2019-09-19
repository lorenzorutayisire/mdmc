#include "LoadScenePhase.hpp"
#include "VisualizeScenePhase.hpp"
#include "../Scene/Scene.hpp"

#include <GL/glew.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <iostream>

LoadScenePhase::LoadScenePhase(const char* path) : path(path)
{
}

void LoadScenePhase::onEnable(PhaseManager* phaseManager)
{
	std::cout << "========================================" << std::endl;
	std::cout << "LoadScenePhase | onEnable" << std::endl;
	std::cout << "========================================" << std::endl;


	// The scene will be cleaned up by the importer destructor.

	Scene* scene = new Scene();
	scene->load(this->path);

	phaseManager->setPhase(new VisualizeScenePhase(scene));
}

void LoadScenePhase::onRender(PhaseManager* phaseManager)
{
	glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
}
