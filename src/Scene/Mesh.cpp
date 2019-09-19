#include "Mesh.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <vector>
#include <iostream>

Mesh::Mesh() :
	vao(NULL),
	vbo(NULL),
	ebo(NULL),

	texture(NULL),
	color(aiColor4D(0)),

	elements_count(0)
{
}

Mesh::~Mesh()
{
}

void Mesh::load_texture(const std::string folder, aiMaterial* ai_material)
{
	glGenTextures(1, &this->texture);
	glBindTexture(GL_TEXTURE_2D, this->texture);

	aiString ai_path;
	if (aiGetMaterialTexture(ai_material, aiTextureType_DIFFUSE, 0, &ai_path) == aiReturn_SUCCESS && ai_path.length > 0)
	{
		int width, height;

		std::string path = folder + "/" + std::string(ai_path.C_Str());
		std::cout << "Image: " << path << std::endl;

		uint8_t* image = stbi_load(path.c_str(), &width, &height, NULL, STBI_rgb_alpha);
		if (!image)
		{
			std::cout << "Error: " << stbi_failure_reason() << std::endl;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

		stbi_image_free(image);
	}
	else
	{
		GLfloat empty_image[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		glBindTexture(GL_TEXTURE_2D, this->texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_FLOAT, empty_image);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Mesh::load_color(aiMaterial* ai_material)
{
	if (aiGetMaterialColor(ai_material, AI_MATKEY_COLOR_DIFFUSE, &color) != aiReturn_SUCCESS)
	{
		this->color = aiColor4D(1.0f, 1.0f, 1.0f, 1.0f);
	}
	printf("Color: %.2f %.2f %.2f %.2f\n", this->color.r, this->color.g, this->color.b, this->color.a);
}

void inline Mesh::load_material(const std::string folder, aiMaterial* ai_material)
{
	this->load_texture(folder, ai_material);
	this->load_color(ai_material);
}

void Mesh::load_vbo(aiMesh* ai_mesh)
{
	std::vector<GLfloat> vertices;
	for (uint32_t i = 0; i < ai_mesh->mNumVertices; i++)
	{
		aiVector3D vertex = ai_mesh->mVertices[i];
		vertices.push_back(vertex.x);
		vertices.push_back(vertex.y);
		vertices.push_back(vertex.z);

		if (ai_mesh->HasTextureCoords(0))
		{
			aiVector3D* texCoords = ai_mesh->mTextureCoords[0];
			vertices.push_back(texCoords[i].x);
			vertices.push_back(1.0f - texCoords[i].y);
		}
		else
		{
			vertices.push_back(0);
			vertices.push_back(0);
		}
	}

	glGenBuffers(1, &this->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
}

void Mesh::load_ebo(aiMesh* ai_mesh)
{
	std::vector<GLuint> indices;
	for (uint32_t i = 0; i < ai_mesh->mNumFaces; i++)
	{
		aiFace face = ai_mesh->mFaces[i];
		if (face.mNumIndices != 3)
		{
			std::cerr << "face.mNumIndices != 3" << std::endl;
			throw;
		}
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	this->elements_count = ai_mesh->mNumFaces * 3;

	glGenBuffers(1, &this->ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
}

void Mesh::load_mesh(aiMesh* ai_mesh)
{
	glGenVertexArrays(1, &this->vao);
	glBindVertexArray(this->vao);

	this->load_vbo(ai_mesh);
	this->load_ebo(ai_mesh);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);
}

void Mesh::load(std::string folder, aiMesh* ai_mesh, aiMaterial* ai_material)
{
	this->load_mesh(ai_mesh);
	this->load_material(folder, ai_material);
}

void Mesh::render()
{
	glBindTexture(GL_TEXTURE_2D, this->texture);

	glUniform4f(2, this->color.r, this->color.g, this->color.b, this->color.a);

	glBindVertexArray(this->vao);
	glDrawElements(GL_TRIANGLES, this->elements_count, GL_UNSIGNED_INT, NULL);
}

void Mesh::destroy()
{
	glDeleteVertexArrays(1, &this->vao);

	glDeleteBuffers(1, &this->vbo);
	glDeleteBuffers(1, &this->ebo);
}
