#pragma once

#include <GL/glew.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <iostream>

#include "GL/Scene.hpp"

#include "VisualizePhase.hpp"

class LoadingPhase : public Phase
{
private:
	const char* path;

public:
	LoadingPhase(const char* path) : path(path) {}

	void on_enable(PhaseManager* phase_manager)
	{
		std::cout << "================================================================" << std::endl;
		std::cout << "Loading scene" << std::endl;
		std::cout << "================================================================" << std::endl;

		Scene* scene = new Scene();
		scene->load(this->path);
		phase_manager->set_phase(new VisualizePhase(scene));
	}
};
