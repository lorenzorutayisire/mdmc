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
	uint16_t height;
	std::istream* minecraft_asset_file;

public:
	LoadingPhase(const char* path, uint16_t height, std::istream& minecraft_asset_file) :
		path(path),
		height(height),
		minecraft_asset_file(&minecraft_asset_file)
	{
	}

	void on_enable(PhaseManager* phase_manager)
	{
		std::cout << "================================================================" << std::endl;
		std::cout << "LoadingPhase" << std::endl;
		std::cout << "================================================================" << std::endl;

		Scene* scene = new Scene();
		scene->load(this->path);
		phase_manager->set_phase(new VisualizePhase(scene, height, *minecraft_asset_file));
	}
};
