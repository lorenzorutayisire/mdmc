#include "octree_builder.hpp"

#include <filesystem>
#include <iostream>

#include <glm/glm.hpp>

#include "util/render_doc.hpp"

// ================================================================================================
// OctreeBuilder
// ================================================================================================

OctreeBuilder::OctreeBuilder()
{
	// node_flag
	{
		Shader shader(GL_COMPUTE_SHADER);
		shader.source_from_file("resources/shaders/svo_node_flag.comp");
		if (!shader.compile())
			throw;

		this->node_flag.attach_shader(shader);
		if (!this->node_flag.link())
		{
			std::cerr << this->node_flag.get_log() << std::endl;
			throw;
		}
	}

	// node_alloc
	{
		Shader shader(GL_COMPUTE_SHADER);
		shader.source_from_file("resources/shaders/svo_node_alloc.comp");
		if (!shader.compile())
			throw;

		this->node_alloc.attach_shader(shader);
		this->node_alloc.link();
	}

	// node_init
	{
		Shader shader(GL_COMPUTE_SHADER);
		shader.source_from_file("resources/shaders/svo_node_init.comp");
		if (!shader.compile())
			throw;

		this->node_init.attach_shader(shader);
		this->node_init.link();
	}

	// store_leaf
	{
		Shader shader(GL_COMPUTE_SHADER);
		shader.source_from_file("resources/shaders/svo_store_leaf.comp");
		if (!shader.compile())
			throw;

		this->store_leaf.attach_shader(shader);
		this->store_leaf.link();
	}
}

std::shared_ptr<Octree> OctreeBuilder::build(std::shared_ptr<VoxelList> const& voxel_list, size_t octree_max_level)
{
	auto octree = std::make_shared<Octree>(octree_max_level);

	AtomicCounter alloc_counter;

	GLuint start = 0;
	GLuint count = 8;
	GLuint alloc_start = start + count;

	const int workgroup_size = 32;

	for (int level = octree_max_level; level > 0; level--)
	{
		// node_flag
		this->node_flag.use();

		glUniform1i(this->node_flag.get_uniform_location("u_max_level"), octree_max_level);
		glUniform1i(this->node_flag.get_uniform_location("u_level"), level);

		octree->bind(1);
		voxel_list->bind(2, 3);

		RenderDoc().capture([&] {
			glDispatchCompute(glm::ceil(voxel_list->size / float(workgroup_size)), 1, 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		});

		this->node_flag.unuse();

		// node_alloc
		this->node_alloc.use();

		glUniform1ui(this->node_alloc.get_uniform_location("u_start"), start);
		glUniform1ui(this->node_alloc.get_uniform_location("u_count"), count);
		glUniform1ui(this->node_alloc.get_uniform_location("u_alloc_start"), alloc_start);

		octree->bind(1);
		alloc_counter.bind(2);

		alloc_counter.set_value(0);

		RenderDoc().capture([&] {
			glDispatchCompute(glm::ceil(count / float(workgroup_size)), 1, 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);
		});

		this->node_alloc.unuse();

		GLuint alloc_count = alloc_counter.get_value();
		start = alloc_start;
		count = alloc_count * 8;
		alloc_start = start + count;

		// By default all octree values are set to 0, no need to initialize.
		/*
		this->node_init.use();

		glUniform1ui(this->node_alloc.get_uniform_location("u_start"), start);
		glUniform1ui(this->node_alloc.get_uniform_location("u_count"), count);

		octree->bind(1);

		RenderDoc().capture([&] {
			glDispatchCompute(glm::ceil(count / float(work_group_size)), 1, 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		});

		this->node_init.unuse();
		*/
	}

	// store_leaf
	this->store_leaf.use();

	glUniform1i(this->store_leaf.get_uniform_location("u_octree_max_level"), octree_max_level);

	octree->bind(1);
	voxel_list->bind(2, 3);

	int workgroup_count = glm::ceil(voxel_list->size / float(workgroup_size));
	RenderDoc().capture([&] {
		glDispatchCompute(workgroup_count, 1, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	});

	this->store_leaf.unuse();

	return octree;
}
