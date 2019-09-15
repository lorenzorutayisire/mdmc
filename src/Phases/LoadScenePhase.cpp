#include "LoadScenePhase.hpp"
#include "VisualizeScenePhase.hpp"

#include <GL/glew.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <iostream>

LoadScenePhase::LoadScenePhase(PhaseManager* phaseManager, const char* path) : Phase(phaseManager), path(path)
{
}

void LoadScenePhase::onEnable()
{
	std::cout << "========================================" << std::endl;
	std::cout << "LoadScenePhase" << std::endl;
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
	phaseManager->setPhase(new VisualizeScenePhase(phaseManager.get(), scene));
}

void LoadScenePhase::onRender()
{
	glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
}
