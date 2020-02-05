#pragma once

#include <vector>
#include <string>
#include <filesystem>

#include <GL/glew.h>

#include <assimp/scene.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

#include "Voxelizer.hpp"

namespace mdmc
{
	enum AttributeLayout
	{
		POSITION  = 0,
		NORMAL	  = 1,
		TEX_COORD = 2,
		COLOR	  = 3,

		size
	};

	struct BakedMesh
	{
		GLuint vao;
		GLuint vbo[AttributeLayout::size];
		GLuint ebo;

		uint32_t elements_count;

		GLuint texture;
		aiColor4D color;
		aiMatrix4x4 transformation;
	};

	class aiSceneWrapper : public Voxelizer::Field
	{
	private:
		std::vector<BakedMesh> baked_meshes;

	public:
		void bake_node(const aiNode* node, const aiScene* scene, aiMatrix4x4 transformation, const std::filesystem::path& textures_path);
		void bake_mesh(const aiMesh* mesh, const aiScene* scene, aiMatrix4x4 transformation, const std::filesystem::path& textures_path);

		aiSceneWrapper(const aiScene* ai_scene, const std::filesystem::path& path);

		void render() const override;
	};
}
