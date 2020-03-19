#include "minecraft_baked_assets.hpp"

// ================================================================================================
// MinecraftBakedAssets
// ================================================================================================

MinecraftBakedAssets::MinecraftBakedAssets()
{
	glGenVertexArrays(1, &this->vao);
	glGenBuffers(1, &this->vbo);
}

MinecraftBakedAssets::~MinecraftBakedAssets()
{
	glDeleteVertexArrays(1, &this->vao);
	glDeleteBuffers(1, &this->vbo);
}

void MinecraftBakedAssets::draw(BufferPosition const& buffer_position) const
{
	glBindVertexArray(this->vao);
	glDrawArrays(
		GL_QUADS,
		std::get<0>(buffer_position),
		std::get<1>(buffer_position)
	);
	glBindVertexArray(0);
}

void MinecraftBakedAssets::draw(unsigned int block_id) const
{
	this->draw(
		this->buffer_position_by_id.at(block_id)
	);
}

void MinecraftBakedAssets::draw(std::string const& block_name) const
{
	this->draw(
		this->buffer_position_by_name.at(block_name)
	);
}

std::shared_ptr<MinecraftBakedAssets const> MinecraftBakedAssets::bake(std::shared_ptr<MinecraftAssets const> const& assets)
{
	auto baked_assets = std::make_shared<MinecraftBakedAssets>();

	std::vector<GLfloat> buffer;
	unsigned int vertices_count = 0;

	for (auto& block_state_variant : assets->block_state_variant_by_id)
	{
		unsigned int start_vertices_count = vertices_count;
		vertices_count += block_state_variant->second->bake(buffer, assets, glm::mat4(1));

		auto buffer_position = std::make_tuple(start_vertices_count, vertices_count - start_vertices_count);
		baked_assets->buffer_position_by_id.push_back(buffer_position);
		baked_assets->buffer_position_by_name.insert(std::make_pair(block_state_variant->first, buffer_position));
	}

	glBindVertexArray(baked_assets->vao);

	glBindBuffer(GL_ARRAY_BUFFER, baked_assets->vbo);
	glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(GLfloat), buffer.data(), GL_STATIC_DRAW);

	const size_t vertex_size = 3 * sizeof(GLfloat) + 2 * sizeof(GLfloat) + 1 * sizeof(GLfloat);
	size_t shift = 0;

	// POSITION
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)shift);
	shift += 3 * sizeof(GLfloat);

	// UV
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertex_size, (void*)shift);
	shift += 2 * sizeof(GLfloat);

	// TINT_INDEX
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, vertex_size, (void*)shift);
	shift += 1 * sizeof(GLfloat);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return baked_assets;
}
