#include "aiSceneWrapper.hpp"

#include <utility>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace mdmc;

void test_min_max(const aiMesh* mesh, const aiMatrix4x4& transform, glm::vec3& min, glm::vec3& max)
{
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		auto v = transform * mesh->mVertices[i]; // min/max must be tested on the transformed Mesh.
		
		if (v.x < min.x) min.x = v.x;
		if (v.y < min.y) min.y = v.y;
		if (v.z < min.z) min.z = v.z;

		if (v.x > max.x) max.x = v.x;
		if (v.y > max.y) max.y = v.y;
		if (v.z > max.z) max.z = v.z;
	}
}

aiSceneWrapper::aiSceneWrapper(const aiScene* scene, const std::filesystem::path& textures_path)
{
	this->min = glm::vec3(0);
	this->max = glm::vec3(0);

	this->bake_node(scene->mRootNode, scene, aiMatrix4x4(), textures_path);
}

void aiSceneWrapper::bake_node(const aiNode* node, const aiScene* scene, aiMatrix4x4 transformation, const std::filesystem::path& textures_path)
{
	transformation *= node->mTransformation;

	for (unsigned int i = 0; i < node->mNumMeshes; i++)
		this->bake_mesh(scene->mMeshes[node->mMeshes[i]], scene, transformation, textures_path);

	for (unsigned int i = 0; i < node->mNumChildren; i++)
		this->bake_node(node->mChildren[i], scene, transformation, textures_path);

}

void aiSceneWrapper::bake_mesh(const aiMesh* mesh, const aiScene* scene, aiMatrix4x4 transformation, const std::filesystem::path& textures_path)
{
	BakedMesh baked_mesh;
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	/* ================================================================= */
	/* VAO/VBO */

	glGenVertexArrays(1, &baked_mesh.vao);
	glBindVertexArray(baked_mesh.vao);

	/* POSITION */

	glGenBuffers(1, &baked_mesh.vbo[AttributeLayout::POSITION]);
	glBindBuffer(GL_ARRAY_BUFFER, baked_mesh.vbo[AttributeLayout::POSITION]);
	glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * 3 * sizeof(GLfloat), mesh->mVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(AttributeLayout::POSITION);
	glVertexAttribPointer(AttributeLayout::POSITION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	test_min_max(mesh, transformation, this->min, this->max);

	/* Transformation */
	baked_mesh.transformation = transformation.Transpose();

	/* NORMAL */

	glGenBuffers(1, &baked_mesh.vbo[AttributeLayout::NORMAL]);
	glBindBuffer(GL_ARRAY_BUFFER, baked_mesh.vbo[AttributeLayout::NORMAL]);

	glEnableVertexAttribArray(AttributeLayout::NORMAL);

	if (mesh->HasNormals())
	{
		glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * 3 * sizeof(GLfloat), mesh->mNormals, GL_STATIC_DRAW);

		glVertexAttribPointer(AttributeLayout::NORMAL, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	}
	else
	{
		GLfloat normals[] = { 0.0f, 0.0f, 0.0f };
		glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(GLfloat), normals, GL_STATIC_DRAW);

		glVertexAttribPointer(AttributeLayout::NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribDivisor(AttributeLayout::NORMAL, mesh->mNumVertices);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* TEX_COORD */

	glGenBuffers(1, &baked_mesh.vbo[AttributeLayout::TEX_COORD]);
	glBindBuffer(GL_ARRAY_BUFFER, baked_mesh.vbo[AttributeLayout::TEX_COORD]);

	glEnableVertexAttribArray(AttributeLayout::TEX_COORD);

	if (mesh->HasTextureCoords(0))
	{
		glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * 3 * sizeof(GLfloat), mesh->mTextureCoords[0], GL_STATIC_DRAW);

		glVertexAttribPointer(AttributeLayout::TEX_COORD, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	}
	else
	{
		GLfloat uv[] = { 0.0f, 0.0f };
		glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(GLfloat), uv, GL_STATIC_DRAW);

		glVertexAttribPointer(AttributeLayout::TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribDivisor(AttributeLayout::TEX_COORD, mesh->mNumVertices);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* COLOR */

	glGenBuffers(1, &baked_mesh.vbo[AttributeLayout::COLOR]);
	glBindBuffer(GL_ARRAY_BUFFER, baked_mesh.vbo[AttributeLayout::COLOR]);

	glEnableVertexAttribArray(AttributeLayout::COLOR);

	if (mesh->HasVertexColors(AttributeLayout::COLOR))
	{
		glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * 4 * sizeof(float), mesh->mColors[0], GL_STATIC_DRAW);

		glVertexAttribPointer(AttributeLayout::COLOR, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	}
	else
	{
		GLfloat color[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(float), color, GL_STATIC_DRAW);

		glVertexAttribPointer(AttributeLayout::COLOR, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribDivisor(AttributeLayout::COLOR, mesh->mNumVertices);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* ================================================================= */
	/* EBO */

	GLuint* indices = new GLuint[mesh->mNumFaces * 3];
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace& face = mesh->mFaces[i];
		if (face.mNumIndices != 3)
		{
			std::cerr << "face.mNumIndices != 3" << std::endl;
			throw;
		}
		indices[i * 3] = face.mIndices[0];
		indices[i * 3 + 1] = face.mIndices[1];
		indices[i * 3 + 2] = face.mIndices[2];
	}
	baked_mesh.elements_count = mesh->mNumFaces * 3;

	glGenBuffers(1, &baked_mesh.ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, baked_mesh.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, baked_mesh.elements_count * sizeof(GLuint), indices, GL_STATIC_DRAW);

	/* ================================================================= */
	/* Color */

	if (aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &baked_mesh.color) != aiReturn_SUCCESS)
		baked_mesh.color = aiColor4D(1.0f, 1.0f, 1.0f, 1.0f);

	/* ================================================================= */
	/* Texture */
	
	glGenTextures(1, &baked_mesh.texture);
	glBindTexture(GL_TEXTURE_2D, baked_mesh.texture);

	aiString texture_path;
	if (aiGetMaterialTexture(material, aiTextureType_DIFFUSE, 0, &texture_path) == aiReturn_SUCCESS && texture_path.length > 0)
	{
		int width, height;
		uint8_t* image_data = stbi_load((textures_path / texture_path.C_Str()).u8string().c_str(), &width, &height, NULL, STBI_rgb_alpha);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

		stbi_image_free(image_data);
	}
	else
	{
		GLfloat empty_image[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_FLOAT, empty_image);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	this->baked_meshes.push_back(baked_mesh);
}

void aiSceneWrapper::render() const
{
	for (auto& baked_mesh : this->baked_meshes)
	{
		/* Texture */
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, baked_mesh.texture);

		// u_color
		auto& color = baked_mesh.color;
		glUniform4f(6, color.r, color.g, color.b, color.a);

		// u_transform
		glUniformMatrix4fv(4, 1, GL_FALSE, baked_mesh.transformation[0]);

		/* VAO */
		glBindVertexArray(baked_mesh.vao);

		/* EBO */
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, baked_mesh.ebo);
		glDrawElements(GL_TRIANGLES, baked_mesh.elements_count, GL_UNSIGNED_INT, NULL);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
