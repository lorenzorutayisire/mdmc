#pragma once

#include <GL/glew.h>
#include <assimp/scene.h>

#include <string>

class Material
{
private:
	GLuint texture;
	aiColor4D color;

public:
	Material();
	~Material();

	void loadAiMaterial(std::string loadingPath, aiMaterial* aiMaterial);

	void bind();
};
