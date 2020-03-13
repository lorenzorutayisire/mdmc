#include "octree.hpp"

Octree::Octree(size_t max_resolution) :
	max_resolution(max_resolution),
	buffer_name([]() {
		GLuint name;
		glGenBuffers(1, &name);
		return name;
	}()),
	texture_name([]() {
		GLuint name;
		glGenTextures(1, &name);
		return name;
	}())
{
	glBindBuffer(GL_TEXTURE_BUFFER, this->buffer_name);

	// At first, the octree allocates the memory it takes in the worst case.
	size_t capacity = Octree::size(max_resolution);
	glBufferData(GL_TEXTURE_BUFFER, capacity, NULL, GL_DYNAMIC_DRAW);

	glBindTexture(GL_TEXTURE_BUFFER, this->texture_name);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32UI, this->buffer_name);

	glBindBuffer(GL_TEXTURE_BUFFER, 0);
}

Octree::~Octree()
{
	glDeleteBuffers(1, &this->texture_name);
	glDeleteBuffers(1, &this->buffer_name);
}

void Octree::bind(GLuint binding)
{
	glBindBufferBase(GL_TEXTURE_BUFFER, binding, this->texture_name);
}

size_t Octree::size(glm::uint level)
{
	size_t res = 0;
	do
	{
		res += pow(8, level) * 8;
		level--;
	} while (level != 0);
	return res;
}