#include "LoadScenePhase.hpp"
#include "VisualizeScenePhase.hpp"

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

	Assimp::Importer importer;

	const aiScene* aiScene = importer.ReadFile(this->path.c_str(), aiProcess_Triangulate);
	if (aiScene == NULL) {
		std::cerr << "Scene could not be loaded: " << importer.GetErrorString() << std::endl;
		throw; // aiScene could not be loaded. Check the file path.
	}

	// The scene will be cleaned up by the importer destructor.

	Scene* scene = new Scene();
	scene->loadAiScene(aiScene);
	phaseManager->setPhase(new VisualizeScenePhase(scene));
}

void LoadScenePhase::onRender(PhaseManager* phaseManager)
{
	glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
}
