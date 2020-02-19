#include "octree.hpp"

Octree::Octree(const TextureBuffer& texture_buffer) :
	texture_buffer(texture_buffer)
{}

OctreeBuilder::OctreeBuilder()
{
	Shader shader(GL_COMPUTE_SHADER);
	shader.source_from_file("resources/shaders/octree_build.comp");
	shader.compile();

	program.attach_shader(shader);

	program.link();
}

std::shared_ptr<const TextureBuffer> OctreeBuilder::build(const Volume& volume)
{
	size_t largest_side = glm::max(
		volume.size.x,
		glm::max(
			volume.size.y,
			volume.size.z
		)
	);

	auto alloc_offset = AtomicCounter::create();
	alloc_offset.set_value(0);

	size_t depth = 0;
	for (; glm::pow(2, depth) >= largest_side; depth++);

	auto octree = std::shared_ptr<TextureBuffer>();

	program.use();

	for (size_t level = 0; level <= depth; level++)
	{
		glUniform1ui(program.get_uniform_location("write"), level == depth);
		glUniform1ui(program.get_uniform_location("level"), level);

		alloc_offset.bind(1);
		glBindImageTexture(0, volume.texture3d.name, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
		glBindImageTexture(2, octree->texture_name, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);

		glDispatchCompute(volume.size.x, volume.size.y, volume.size.z);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);
	}

	program.unuse();

	return octree;
}

