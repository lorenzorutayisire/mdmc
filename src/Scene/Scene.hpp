#pragma once

#include "Mesh.hpp"

#include <vector>
#include <string>

#include <assimp/scene.h>

class Scene
{
private:
	std::vector<Mesh> meshes;

public:
	Scene();
	~Scene();

	void load(const std::string path);

	void render();

	void destroy();
};
