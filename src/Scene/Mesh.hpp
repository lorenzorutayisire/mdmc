#pragma once

#include <GL/glew.h>
#include <assimp/scene.h>

class Mesh
{
private:
	GLuint vao, vbo, ebo;
	uint32_t elements_count;

	GLuint texture;
	aiColor4D color;

public:
	Mesh();
	~Mesh();

	GLuint get_vao() { return this->vao; }
	GLuint get_vbo() { return this->vbo; }
	GLuint get_ebo() { return this->ebo;  }

	uint32_t get_elements_count() { return this->elements_count;  }

	GLuint get_texture() { return this->texture;  }
	aiColor4D& get_color() { return this->color;  }

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
