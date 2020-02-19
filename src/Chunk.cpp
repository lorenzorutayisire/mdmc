#include "chunk.hpp"

// ================================================================================================

Chunk::Chunk(const std::shared_ptr<const Field>& field, glm::uvec3 grid, glm::uvec3 chunk_position, glm::uvec3 chunk_grid) :
	Field(
		field->min + glm::vec3(chunk_position) * (field->size / glm::vec3(grid) * glm::vec3(chunk_grid)),
		glm::max(this->min + field->size / glm::vec3(grid) * glm::vec3(chunk_grid), field->max)
	),
	field(field),
	position(chunk_position),
	grid(
		glm::max(chunk_position * chunk_grid, grid)
	)
{}

void Chunk::render() const
{
	this->field->render();
}

// ================================================================================================


glm::uvec3 ChunkSplitter::get_chunk_grid(glm::uvec3 grid) const
{
	const glm::uint max_chunk_size = 1024 * 1024 * 1024;

	glm::uint chunk_size = (grid.x * grid.y * grid.z) / max_chunk_size;
	return glm::ceil(glm::vec3(grid) / (float) cbrt(chunk_size));
}

ChunkSplitter::ChunkSplitter(const std::shared_ptr<const Field>& field, glm::uvec3 grid) :
	field(field),
	grid(grid),
	chunk_size(get_chunk_grid(grid)),
	chunk_count(glm::ceil(field->size / chunk_size))
{}

std::shared_ptr<const Chunk> ChunkSplitter::get_chunk(glm::uvec3 chunk_position) const
{
	return nullptr;
}

ChunkSplitter ChunkSplitter::from_height(const std::shared_ptr<const Field>& field, glm::uint height)
{
	float ratio = height / field->size.y;
	glm::uvec3 proportional_grid = ratio * field->size;

	return ChunkSplitter(field, proportional_grid);
}

