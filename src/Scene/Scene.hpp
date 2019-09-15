#pragma once

#include "Material.hpp"
#include "Mesh.hpp"

#include <assimp/scene.h>
#include <vector>

class Scene
{
private:
	std::vector<Material> materials;
	std::vector<Mesh> meshes;

public:
	Scene();
	~Scene();

	void loadAiScene(const aiScene* aiScene);

	void render();
};
