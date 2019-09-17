#include "Mesh.hpp"

#include <vector>

Mesh::Mesh() : elementsCount(0), material(new Material())
{
	glGenBuffers(1, &this->vao);
	glGenBuffers(1, &this->vbo);
	glGenBuffers(1, &this->ebo);
}

Mesh::~Mesh()
{
	glDeleteBuffers(1, &this->vao);
	glDeleteBuffers(1, &this->vbo);
	glDeleteBuffers(1, &this->ebo);
}

void Mesh::loadAiMesh(aiMesh* aiMesh)
{
	glBindVertexArray(this->vao);

	std::vector<float> vertices;
	for (uint32_t i = 0; i < aiMesh->mNumVertices; i++)
	{
		aiVector3D vertex = aiMesh->mVertices[i];
		vertices.push_back(vertex.x);
		vertices.push_back(vertex.y);
		vertices.push_back(vertex.z);

		aiVector3D* texCoords = aiMesh->mTextureCoords[0];
		if (aiMesh->HasTextureCoords(0))
		{
			vertices.push_back(texCoords[i].x);
			vertices.push_back(texCoords[i].y);
		}
		else
		{
			vertices.push_back(0);
			vertices.push_back(0);
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	std::vector<unsigned int> indices;
	for (uint32_t i = 0; i < aiMesh->mNumFaces; i++)
	{
		aiFace face = aiMesh->mFaces[i];
		if (face.mNumIndices != 3)
		{
			throw; // Invalid face indices number, should be 3.
		}
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}
	this->elementsCount = aiMesh->mNumFaces * 3;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size(), indices.data(), GL_STATIC_DRAW);
}

void Mesh::setMaterial(Material* material)
{
	this->material = material;
}

void Mesh::render()
{
	this->material->bind();

	glBindVertexArray(this->vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);

	glDrawElements(GL_TRIANGLES, this->elementsCount, GL_UNSIGNED_INT, NULL);
}
