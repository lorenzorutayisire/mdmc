#pragma once

#include "Mesh.hpp"

#include <vector>
#include <string>

#include <assimp/scene.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Scene
{
private:
	glm::vec3 min_vertex, max_vertex;

	std::vector<Mesh> meshes;

	void test_min_vertex(Mesh mesh);
	void test_max_vertex(Mesh mesh);

public:
	Scene();
	~Scene();

	std::vector<Mesh>& get_meshes();

	glm::vec3 get_min_vertex() { return this->min_vertex; }
	glm::vec3 get_max_vertex() { return this->max_vertex; }
	glm::vec3 get_size() { return this->max_vertex - this->min_vertex;  }

	void load(const std::string path);

	void render();

	void destroy();
};