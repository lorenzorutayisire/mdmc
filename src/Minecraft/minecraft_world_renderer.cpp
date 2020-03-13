#include "minecraft_world_renderer.hpp"

MinecraftWorldRenderer::MinecraftWorldRenderer(const std::shared_ptr<MinecraftVersionPool>& version_pool) :
	version_pool(version_pool),
	vbo([]() {
		GLuint name;
		glGenBuffers(1, &name);
		return name;
	}())
{}

MinecraftWorldRenderer::~MinecraftWorldRenderer()
{
	glDeleteBuffers(1, &this->vbo);
}

const MinecraftBlockStateVariant* MinecraftWorldRenderer::get_block(const glm::uvec3& position) const
{
	if (this->blocks_by_position.find(position) != this->blocks_by_position.end())
		return this->blocks_by_position.at(position);
	
	auto air_block = this->version_pool->get_block_state_variant(0);
	return &air_block.second;
}

void MinecraftWorldRenderer::set_block(const glm::uvec3& position, const MinecraftBlockStateVariant& block_state_variant, bool must_build)
{
	//this->blocks_by_position.insert(std::make_pair(position, &block_state_variant));
	if (must_build)
		this->build();
}

void MinecraftWorldRenderer::build()
{
	const size_t vertex_size = 3 * sizeof(GLfloat) + 1 * sizeof(GLbyte);

	// Count
	size_t elements_count = 0;
	for (auto& member : this->blocks_by_position)
		elements_count += member.second->get_model(this->version_pool).elements.size();

	size_t size = elements_count * 6 * 4 * vertex_size;
}
