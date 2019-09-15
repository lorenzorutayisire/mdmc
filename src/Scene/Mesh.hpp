#pragma once

#include "Material.hpp"

#include <GL/glew.h>
#include <assimp/scene.h>

class Mesh
{
private:
	GLuint vao, vbo, ebo;
	unsigned int elementsCount;

	Material* material;

public:
	Mesh();
	~Mesh();

	void loadAiMesh(aiMesh* aiMesh);

	void setMaterial(Material* material);

	void render();
};
