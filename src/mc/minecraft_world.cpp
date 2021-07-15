#include "minecraft_world.hpp"

#include <stb_image.h>

#include <glm/gtx/transform.hpp>

#ifdef IGNORED

MinecraftWorld::MinecraftWorld(std::shared_ptr<MinecraftContext const> const& context) :
	context(context)
{
	glGenVertexArrays(1, &this->vao);
	glGenBuffers(1, &this->vbo);
}

MinecraftWorld::~MinecraftWorld()
{
	glDeleteBuffers(1, &this->vao);
	glDeleteBuffers(1, &this->vbo);
}

MinecraftBlockStateVariant const* MinecraftWorld::get_block(glm::ivec3 const& position) const
{
	if (this->block_by_position.find(position) != this->block_by_position.end())
		return this->block_by_position.at(position);

	auto air = this->context->assets->block_state_by_name.at("air");
	return &air.variant_by_name.at("");
}

void MinecraftWorld::set_block(glm::ivec3 const& position, MinecraftBlockStateVariant const& block_state_variant, bool rebuild)
{
	this->block_by_position.insert(std::make_pair(position, &block_state_variant));
	if (rebuild)
		this->build();
}

void MinecraftWorld::build()
{
	std::vector<GLfloat> buffer;
	this->vertices_count = 0;

	for (auto& member : this->block_by_position)
	{
		glm::vec3 position = member.first;
		MinecraftBlockStateVariant const* block_state = member.second;

		glm::mat4 transform(1);
		transform = glm::translate(transform, position);
		transform = glm::scale(transform, glm::vec3(1.0f / 16.0f));

		this->vertices_count += block_state->bake(buffer, this->context->assets, transform);
	}

	glBindVertexArray(this->vao);

	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(GLfloat), buffer.data(), GL_STATIC_DRAW);

	const size_t vertex_size = 3 * sizeof(GLfloat) + 2 * sizeof(GLfloat) + 1 * sizeof(GLfloat);
	size_t shift = 0;

	glEnableVertexAttribArray(VertexAttribute::POSITION);
	glVertexAttribPointer(VertexAttribute::POSITION, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)shift);
	shift += 3 * sizeof(GLfloat);

	glEnableVertexAttribArray(VertexAttribute::UV);
	glVertexAttribPointer(VertexAttribute::UV, 2, GL_FLOAT, GL_FALSE, vertex_size, (void*)shift);
	shift += 2 * sizeof(GLfloat);

	glEnableVertexAttribArray(VertexAttribute::TINT_INDEX);
	glVertexAttribPointer(VertexAttribute::TINT_INDEX, 1, GL_FLOAT, GL_FALSE, vertex_size, (void*)shift);
	shift += 1 * sizeof(GLfloat);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void MinecraftWorld::draw()
{
	glBindTexture(GL_TEXTURE_2D, this->context->get_atlas_texture());

	glBindVertexArray(this->vao);
	glDrawArrays(GL_QUADS, 0, this->vertices_count);
}

#endif
