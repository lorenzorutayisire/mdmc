#pragma once

#include <GL/glew.h>
#include <assimp/scene.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Mesh
{
private:
	GLuint vao, vbo, ebo;
	uint32_t elements_count;

	glm::vec3 min_vertex, max_vertex;

	GLuint texture;
	aiColor4D color;

	void test_min_vertex(aiVector3D vertex);
	void test_max_vertex(aiVector3D vertex);

public:
	Mesh();
	Mesh(const Mesh& mesh) = default;
	~Mesh();

	GLuint get_vao() { return this->vao; }
	GLuint get_vbo() { return this->vbo; }
	GLuint get_ebo() { return this->ebo;  }

	uint32_t get_elements_count() { return this->elements_count;  }

	GLuint get_texture() { return this->texture;  }
	aiColor4D& get_color() { return this->color;  }

	glm::vec3 get_min_vertex() { return this->min_vertex; }
	glm::vec3 get_max_vertex() { return this->max_vertex; }

	void load_texture(const std::string folder, aiMaterial* ai_material);
	void load_color(aiMaterial* ai_material);
	void load_material(const std::string folder, aiMaterial* ai_material);

	void load_vbo(aiMesh* ai_mesh, aiMatrix4x4 transform);
	void load_ebo(aiMesh* ai_mesh);
	void load_mesh(aiMesh* ai_mesh, aiMatrix4x4 transform);

	void load(std::string folder, aiMesh* aiMesh, aiMaterial* ai_material, aiMatrix4x4 transform);

	void render();

	void destroy();
};
