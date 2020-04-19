#include "minecraft_baked_block_pool.hpp"

#define VERTEX_SIZE (3 + 2 + 2 + 1)

// ================================================================================================
// MinecraftBakedBlock
// ================================================================================================

MinecraftBakedBlock::MinecraftBakedBlock(
	MinecraftBakedBlockPool const* pool,
	unsigned int start_at,
	unsigned int count
) :
	pool(pool),
	start_at(start_at),
	count(count)
{}

void MinecraftBakedBlock::draw() const
{
	glBindVertexArray(pool->vao);
	glDrawArrays(GL_QUADS, this->start_at, this->count);
	glBindVertexArray(0);
}

// ================================================================================================
// MinecraftBakedBlockPool
// ================================================================================================

MinecraftBakedBlockPool::MinecraftBakedBlockPool()
{
	glGenVertexArrays(1, &this->vao);
	glGenBuffers(1, &this->vbo);
}

MinecraftBakedBlockPool::~MinecraftBakedBlockPool()
{
	glDeleteVertexArrays(1, &this->vao);
	glDeleteBuffers(1, &this->vbo);
}

MinecraftBakedBlock const& MinecraftBakedBlockPool::get_block(unsigned int block_id) const
{
	return block_by_id.at(block_id);
}

MinecraftBakedBlock const& MinecraftBakedBlockPool::get_block(std::string const& block_name) const
{
	return block_by_name.at(block_name);
}

void MinecraftBakedBlockPool::bake(std::shared_ptr<MinecraftAssets const> const& assets)
{
	std::vector<GLfloat> buffer;
	unsigned int start_at = 0;

	for (auto& block : assets->block_state_variant_by_id)
	{
		unsigned int count = block->second->bake(buffer, assets, glm::mat4(1));

		MinecraftBakedBlock baked_block(this, start_at, count);
		this->block_by_id.push_back(baked_block);
		this->block_by_name.insert(std::make_pair(block->first, baked_block));

		start_at += count;
	}

	glBindVertexArray(this->vao);

	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(GLfloat), buffer.data(), GL_STATIC_DRAW);

	GLuint loc;
	size_t shift = 0;

	// Position
	loc = 0;
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, VERTEX_SIZE * sizeof(GLfloat), (void*) shift);
	shift += 3 * sizeof(GLfloat);

	// Tile
	loc = 1;
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE * sizeof(GLfloat), (void*) shift);
	shift += 2 * sizeof(GLfloat);

	// Uv
	loc = 2;
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE * sizeof(GLfloat), (void*) shift);
	shift += 2 * sizeof(GLfloat);

	// Tint
	loc = 3;
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 1, GL_FLOAT, GL_FALSE, VERTEX_SIZE * sizeof(GLfloat), (void*) shift);
	shift += 1 * sizeof(GLfloat);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
