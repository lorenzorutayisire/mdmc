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

void Scene::test_min_vertex(Mesh mesh)
{
	glm::vec3 mesh_min_vertex = mesh.get_min_vertex();
	this->min_vertex.x = (this->min_vertex.x < mesh_min_vertex.x) ? this->min_vertex.x : mesh_min_vertex.x;
	this->min_vertex.y = (this->min_vertex.y < mesh_min_vertex.y) ? this->min_vertex.y : mesh_min_vertex.y;
	this->min_vertex.z = (this->min_vertex.z < mesh_min_vertex.z) ? this->min_vertex.z : mesh_min_vertex.z;

}

void Scene::test_max_vertex(Mesh mesh)
{
	glm::vec3 mesh_max_vertex = mesh.get_max_vertex();
	this->max_vertex.x = (this->max_vertex.x > mesh_max_vertex.x) ? this->max_vertex.x : mesh_max_vertex.x;
	this->max_vertex.y = (this->max_vertex.y > mesh_max_vertex.y) ? this->max_vertex.y : mesh_max_vertex.y;
	this->max_vertex.z = (this->max_vertex.z > mesh_max_vertex.z) ? this->max_vertex.z : mesh_max_vertex.z;
}

std::vector<Mesh>& Scene::get_meshes()
{
	return this->meshes;
}

void Scene::load(const std::string path)
{
	Assimp::Importer importer;

	const aiScene* ai_scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate);
	if (ai_scene == NULL) {
		std::cerr << "Scene loading issue: " << importer.GetErrorString() << std::endl;
		throw;
	}

	Mesh m_a;
	Mesh m_b = m_a;

	this->min_vertex = glm::vec3(0);
	this->max_vertex = glm::vec3(0);

	std::string folder = path.substr(0, path.find_last_of("/\\"));


	for (uint32_t i = 0; i < ai_scene->mNumMeshes; i++)
	{
		aiMesh* ai_mesh = ai_scene->mMeshes[i];
		aiMaterial* ai_material = ai_scene->mMaterials[ai_mesh->mMaterialIndex];

		Mesh mesh;

		mesh.load(folder, ai_mesh, ai_material);
		
		this->test_min_vertex(mesh);
		this->test_max_vertex(mesh);

		this->meshes.push_back(mesh);

		std::cout << "Mesh loaded" << std::endl;
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