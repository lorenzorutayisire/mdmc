#include "mc_blocks_voxelizer.hpp"

#include "mc_baker.hpp"
#include "util/render_doc.hpp"

#include <glm/gtc/type_ptr.hpp>


// ------------------------------------------------------------------------------------------------ baked_mc_blocks

mdmc::baked_mc_blocks::baked_mc_blocks()
{
	glGenTextures(1, &m_atlas_texture);

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
}

mdmc::baked_mc_blocks::~baked_mc_blocks()
{
	glDeleteTextures(1, &m_atlas_texture);

	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
}

void mdmc::baked_mc_blocks::from_mc_assets(mdmc::mc_assets const& assets)
{
	mdmc::bake_mc_atlas(m_atlas_texture, assets.m_atlas);

	std::vector<GLfloat> buffer;
	size_t start_at = 0;

	for (auto& variant : assets.m_block_state_variant_by_name)
	{
		size_t vert_count = mdmc::bake_mc_block_state_variant(assets, variant.second, glm::identity<glm::mat4>(), buffer);

		mdmc::baked_mc_blocks::baked_block entry(*this);
		entry.m_start_at = start_at;
		entry.m_count = vert_count;
		m_blocks.push_back(entry);

		start_at += vert_count;
	}

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) (buffer.size() * sizeof(GLfloat)), buffer.data(), GL_STATIC_DRAW);

	GLuint loc;
	size_t shift = 0;

	// Position
	loc = 0;
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, MDMC_VERTEX_SIZE * sizeof(GLfloat), (void*) shift);
	shift += 3 * sizeof(GLfloat);

	// Tile
	loc = 1;
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, MDMC_VERTEX_SIZE * sizeof(GLfloat), (void*) shift);
	shift += 2 * sizeof(GLfloat);

	// Uv
	loc = 2;
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, MDMC_VERTEX_SIZE * sizeof(GLfloat), (void*) shift);
	shift += 2 * sizeof(GLfloat);

	// Tint
	loc = 3;
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 1, GL_FLOAT, GL_FALSE, MDMC_VERTEX_SIZE * sizeof(GLfloat), (void*) shift);
	shift += 1 * sizeof(GLfloat);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// ------------------------------------------------------------------------------------------------ mc_blocks_voxelizer

mdmc::mc_blocks_voxelizer::mc_blocks_voxelizer()
{
	Shader shader(GL_COMPUTE_SHADER);
	shader.source_from_file("resources/shaders/mc_block_collision_test.comp");
	shader.compile();
	m_program.attach_shader(shader);
	m_program.link();
}

std::shared_ptr<VoxelList> mdmc::mc_blocks_voxelizer::voxelize(mdmc::baked_mc_blocks::baked_block const& block, uint32_t volume_side)
{
	m_program.use();

	glUniform1ui (m_program.get_uniform_location("u_start"), block.m_start_at);
	glUniform1ui (m_program.get_uniform_location("u_count"), block.m_count);
	glUniform3uiv(m_program.get_uniform_location("u_size"), 1, glm::value_ptr(glm::uvec3(volume_side)));

	glBindTexture(GL_TEXTURE_2D, block.m_pool.get().m_atlas_texture);

	AtomicCounter counter;
	counter.bind(1);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, block.m_pool.get().m_vbo);

	glm::uvec3 workgroup_count = glm::ceil(glm::vec3((float) volume_side) / glm::vec3(8));

	// Count
	glUniform1ui(m_program.get_uniform_location("u_store"), 0);
	counter.set_value(0);

	RenderDoc().capture([&] {
		glDispatchCompute(workgroup_count.x, workgroup_count.y, workgroup_count.z);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);
	});

	size_t size = counter.get_value();
	auto voxel_list = std::make_shared<VoxelList>(size);

	// Store
	glUniform1ui(m_program.get_uniform_location("u_store"), 1);
	voxel_list->bind(2, 3);

	counter.set_value(0);

	RenderDoc().capture([&] {
		glDispatchCompute(workgroup_count.x, workgroup_count.y, workgroup_count.z);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);
	});

	Program::unuse();

	return voxel_list;
}
