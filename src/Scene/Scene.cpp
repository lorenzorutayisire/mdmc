#include "Scene.hpp"

#include <utility>

Scene::Scene()
{
}

Scene::~Scene()
{
	this->meshes.clear();
}

void Scene::loadAiScene(const aiScene *aiScene)
{
	// Materials
	for (uint32_t i = 0; i < aiScene->mNumMaterials; i++)
	{
		Material material;
		material.loadAiMaterial(aiScene->mMaterials[i]);
		this->materials.push_back(std::move(material));
	}

	// Meshes
	for (uint32_t i = 0; i < aiScene->mNumMeshes; i++)
	{
		aiMesh* aiMesh = aiScene->mMeshes[i];

		Mesh mesh;
		mesh.loadAiMesh(aiMesh);
		if (aiMesh->mMaterialIndex > 0 && aiMesh->mMaterialIndex < this->meshes.size())
		{
			mesh.setMaterial(&this->materials[i]);
		}
		this->meshes.push_back(std::move(mesh));
	}
}

void Scene::render()
{
	for (Mesh mesh : this->meshes)
	{
		mesh.render();
	}
}
