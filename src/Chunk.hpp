#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "voxelizer.hpp"

// ================================================================================================

struct Chunk : public Field
{
	const std::shared_ptr<const Field> field;
	const glm::uvec3 position, grid;

	Chunk(const std::shared_ptr<const Field>& field, glm::uvec3 grid, glm::uvec3 chunk_position, glm::uvec3 chunk_grid);

	void render() const override;
};

// ================================================================================================

struct ChunkSplitter
{
private:
	glm::vec3 chunk_size;

	virtual glm::uvec3 get_chunk_grid(glm::uvec3 grid) const;

public:
	const std::shared_ptr<const Field> field;
	const glm::uvec3 grid;

	const glm::uvec3 chunk_count;

	ChunkSplitter(const std::shared_ptr<const Field>& field, glm::uvec3 grid);

	std::shared_ptr<const Chunk> get_chunk(glm::uvec3 chunk_position) const;

	static ChunkSplitter from_height(const std::shared_ptr<const Field>& field, glm::uint height);
};
