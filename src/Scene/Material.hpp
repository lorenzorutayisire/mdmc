#pragma once

#include <GL/glew.h>

#include <assimp/scene.h>

class Material
{
private:
	GLuint texture;
	aiColor4D color;

public:
	Material();
	~Material();

	void loadAiMaterial(aiMaterial* aiMaterial);

	void bind();
};
