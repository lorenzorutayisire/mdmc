#include "minecraft_block_voxelizer.hpp"

#include "util/render_doc.hpp"

#include <glm/gtc/type_ptr.hpp>

#define WORKGROUP_SIZE glm::vec3(8, 8, 8);

MinecraftBlockVoxelizer::MinecraftBlockVoxelizer()
{
	Shader shader(GL_COMPUTE_SHADER);
	shader.source_from_file("resources/shaders/mc_block_collision_test.comp");
	shader.compile();
	this->program.attach_shader(shader);

	this->program.link();
}

std::shared_ptr<VoxelList> MinecraftBlockVoxelizer::voxelize(MinecraftBakedBlock const& block, unsigned int side)
{
	this->program.use();

	glUniform1ui(this->program.get_uniform_location("u_start"), block.start_at);
	glUniform1ui(this->program.get_uniform_location("u_count"), block.count);

	AtomicCounter counter;
	counter.bind(1);

	glUniform3uiv(this->program.get_uniform_location("u_size"), 1, glm::value_ptr(glm::uvec3(side)));

	glm::uvec3 workgroup_count = glm::ceil(glm::vec3(side) / glm::vec3(8));
	std::cout << "Dispatching " << workgroup_count.x << "^3 = " << glm::pow(workgroup_count.x, 3) << " workgroups..." << std::endl;

	// Count
	glUniform1ui(this->program.get_uniform_location("u_store"), 0);
	counter.set_value(0);


	RenderDoc().capture([&] {
		glDispatchCompute(workgroup_count.x, workgroup_count.y, workgroup_count.z);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);
	});

	size_t size = counter.get_value();
	auto voxel_list = std::make_shared<VoxelList>(size);

	// Store
	glUniform1ui(this->program.get_uniform_location("u_store"), 1);
	voxel_list->bind(2, 3);

	counter.set_value(0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, block.pool->vbo);

	RenderDoc().capture([&] {
		glDispatchCompute(workgroup_count.x, workgroup_count.y, workgroup_count.z);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);
	});

	this->program.unuse();

	return voxel_list;
}
