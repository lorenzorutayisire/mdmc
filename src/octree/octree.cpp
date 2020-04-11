#include "octree.hpp"

#include <iostream>
#include <stdexcept>

size_t calc_size(unsigned int resolution)
{
	size_t result = 0;
	for (int level = 1; level <= resolution; level++)
		result += glm::exp2(3 * level);
	return result;
}

Octree::Octree(unsigned int resolution) :
	resolution(resolution),
	size(calc_size(resolution)),
	capacity(this->size * sizeof(GLuint))
{
	if (resolution < 1)
		throw std::invalid_argument("resolution must be >= 1");

	this->buffer.load_data(this->capacity, NULL, GL_DYNAMIC_DRAW);
	this->buffer.set_format(GL_R32UI);
}

void Octree::bind(GLuint binding)
{
	this->buffer.bind(binding, 0, GL_FALSE, NULL, GL_READ_WRITE, GL_R32UI);
}
