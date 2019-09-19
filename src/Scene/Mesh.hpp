#pragma once

#include <GL/glew.h>
#include <assimp/scene.h>

class Mesh
{
private:
	GLuint vao, vbo, ebo;
	unsigned int elements_count;

	GLuint texture;
	aiColor4D color;

public:
	Mesh();
	~Mesh();

	void load_texture(const std::string folder, aiMaterial* ai_material);
	void load_color(aiMaterial* ai_material);
	void load_material(const std::string folder, aiMaterial* ai_material);

	void load_vbo(aiMesh* ai_mesh);
	void load_ebo(aiMesh* ai_mesh);
	void load_mesh(aiMesh* ai_mesh);

	void load(std::string folder, aiMesh* aiMesh, aiMaterial* ai_material);

	void render();

	void destroy();
};
