#include "Voxelizer.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <iostream>
#include <fstream>

#include "debug.hpp"

#include <chrono>

#define DEBUG

// ================================================================================================
// Field
// ================================================================================================

glm::vec3 Voxelizer::Field::size() const
{
	return this->max - this->min;
}

float Voxelizer::Field::largest_side() const
{
	glm::vec3 size = this->size();
	return std::max(size.x, std::max(size.y, size.z));
}

glm::mat4 Voxelizer::Field::transform() const
{
	glm::mat4 transform = glm::mat4(1.0);
	transform = glm::scale(transform, glm::vec3(1 / this->largest_side()));
	transform = glm::translate(transform, -this->min);
	return transform;
}

glm::mat4 Voxelizer::Field::x_proj() const
{
	glm::mat4 ortho = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 2.0f);
	return (ortho * glm::lookAt(glm::vec3(-1, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0))) * this->transform();
}

glm::mat4 Voxelizer::Field::y_proj() const
{
	glm::mat4 ortho = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 2.0f);
	return ortho * glm::lookAt(glm::vec3(0, -1, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1)) * this->transform();
}

glm::mat4 Voxelizer::Field::z_proj() const
{
	glm::mat4 ortho = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 2.0f);
	return ortho * glm::lookAt(glm::vec3(0, 0, 2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)) * this->transform();
}

Volume::Volume() :
	data(ShaderStorageBuffer::create())
{}

// ================================================================================================
// Voxelizer
// ================================================================================================

Voxelizer::Voxelizer() :
	program(Program::create())
{
	// Vertex
	auto v_shader = Shader::create(GL_VERTEX_SHADER);
	v_shader.source_from_file("resources/shaders/voxelize.vert");
	if (!v_shader.compile())
	{
		std::cerr << v_shader.get_log() << std::endl;
		throw;
	}
	this->program.attach_shader(v_shader);

	// Geometry
	auto g_shader = Shader::create(GL_GEOMETRY_SHADER);
	g_shader.source_from_file("resources/shaders/voxelize.geom");
	if (!g_shader.compile())
	{
		std::cerr << g_shader.get_log() << std::endl;
		throw;
	}
	this->program.attach_shader(g_shader);

	// Fragment
	auto f_shader = Shader::create(GL_FRAGMENT_SHADER);
	f_shader.source_from_file("resources/shaders/voxelize.frag");
	if (!f_shader.compile())
	{
		std::cerr << f_shader.get_log() << std::endl;
		throw;
	}
	this->program.attach_shader(f_shader);

	// Program
	if (!this->program.link())
	{
		std::cerr << this->program.get_log() << std::endl;
		throw;
	}
}

struct Voxel
{
	GLuint position[4];
	GLfloat color[4];
};

GLuint Voxelizer::sort(std::shared_ptr<const Volume> volume)
{
	auto program = Program::create();

	{
		auto shader = Shader::create(GL_COMPUTE_SHADER);
		shader.source_from_file("resources/shaders/sort_blocks.comp");
		shader.compile();

		program.attach_shader(shader);
		program.link();
	}

	program.use();

	volume->data.bind(0);

	auto swap_count_buf = AtomicCounter::create();
	swap_count_buf.set_value(0);
	swap_count_buf.bind(1);

	GLuint swap_odd = 0, swap_count;
	GLuint passes = 0;

	while (true)
	{
		swap_count_buf.set_value(0);

		glUniform1ui(program.get_uniform_location("volume_size"), volume->count);
		glUniform1ui(program.get_uniform_location("resolution"), volume->resolution);
		glUniform1ui(program.get_uniform_location("swap_odd"), swap_odd);

		GLuint work_groups = floor(volume->count / 2);
		glDispatchCompute(work_groups, 1, 1);

		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);

		swap_count = swap_count_buf.get_value();
		swap_odd = !swap_odd; // invert odd to even and viceversa

		passes++;
		if (passes % 10000 == 0)
			std::cout << "passes: " << passes << " - " << "voxel count: " << volume->count << " - " << "swap count: " << swap_count << std::endl;

		if (passes > 1 && swap_count == 0)
			break;
	}

#ifdef DEBUG
	auto debug = RenderDoc::init();
	debug.start_capture();

	glDispatchCompute(1, 1, 1);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);

	debug.end_capture();
#endif

	return passes;
}

std::shared_ptr<const Volume> Voxelizer::voxelize(std::shared_ptr<const Field> field, GLuint height, GLuint resolution)
{
	float ratio = height / field->size().y;
	glm::uvec3 size = glm::ceil(ratio * field->size());

	unsigned int largest_side = glm::max(size.x, glm::max(size.y, size.z));

#ifdef GL_NV_conservative_raster
	//glEnable(GL_CONSERVATIVE_RASTERIZATION_NV); TODO
#endif

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	GLsizei viewport_side = largest_side * resolution;
	glViewport(0, 0, viewport_side, viewport_side);
	
	GLuint empty_framebuffer;
	glGenFramebuffers(1, &empty_framebuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, empty_framebuffer);

	glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH, viewport_side);
	glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, viewport_side);

	this->program.use();

	// Transform
	glUniformMatrix4fv(this->program.get_uniform_location("u_transform"), 1, GL_FALSE, glm::value_ptr(field->transform()));

	// Projections
	glUniformMatrix4fv(this->program.get_uniform_location("u_x_ortho_projection"), 1, GL_FALSE, glm::value_ptr(field->x_proj()));
	glUniformMatrix4fv(this->program.get_uniform_location("u_y_ortho_projection"), 1, GL_FALSE, glm::value_ptr(field->y_proj()));
	glUniformMatrix4fv(this->program.get_uniform_location("u_z_ortho_projection"), 1, GL_FALSE, glm::value_ptr(field->z_proj()));

	glm::vec3 vsize = size * resolution;

	glUniform1f(this->program.get_uniform_location("u_viewport_side"), (GLfloat)viewport_side);
	glUniform3f(this->program.get_uniform_location("u_volume_size"), vsize.x, vsize.y, vsize.z);

	// ================================================================= fragments_size

	auto volume = std::make_shared<Volume>();

	volume->resolution = resolution;

	/* Count */
	glUniform1ui(this->program.get_uniform_location("can_store"), 0);

	auto count_buf = AtomicCounter::create();
	count_buf.set_value(0);
	count_buf.bind(1);

	field->render();

	glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);

	volume->count = count_buf.get_value();

	/* Store */
	auto debug = RenderDoc::init();
	debug.start_capture();

	glUniform1ui(this->program.get_uniform_location("can_store"), 1);

	count_buf.set_value(0);
	count_buf.bind(1);

	volume->data.load_data(sizeof(Voxel) * volume->count, NULL, GL_DYNAMIC_DRAW);
	volume->data.bind(2);
	glClearNamedBufferData(volume->data.name, GL_R8UI, GL_R, GL_UNSIGNED_BYTE, NULL);

	field->render();

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);

	debug.end_capture();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &empty_framebuffer);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	return volume;
}
