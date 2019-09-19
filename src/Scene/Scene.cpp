#include "Scene.hpp"

#include <utility>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::load(const std::string path)
{
	Assimp::Importer importer;

	const aiScene* ai_scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate);
	if (ai_scene == NULL) {
		std::cerr << "Scene loading issue: " << importer.GetErrorString() << std::endl;
		throw;
	}

	Mesh mesh;
	std::string folder = path.substr(0, path.find_last_of("/\\"));

	for (uint32_t i = 0; i < ai_scene->mNumMeshes; i++)
	{
		aiMesh* ai_mesh = ai_scene->mMeshes[i];
		aiMaterial* ai_material = ai_scene->mMaterials[ai_mesh->mMaterialIndex];

		mesh.load(folder, ai_mesh, ai_material);

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

void Scene::destroy()
{
	for (Mesh mesh : this->meshes)
	{
		mesh.destroy();
	}
	this->meshes.clear();
}
