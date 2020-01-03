#include "Chunk.hpp"

using namespace mdmc;

Chunk::Chunk(const std::shared_ptr<const Voxelizer::Field>& field, glm::vec3 from, float side)
{
	this->field = field;

	this->min = from;
	this->max = from + side;
}

void Chunk::render() const
{
	this->field->render();
}
