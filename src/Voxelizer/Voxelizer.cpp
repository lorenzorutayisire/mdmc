#include "Voxelizer.hpp"

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <iostream>
#include <fstream>

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

GLuint program2;

bool init;

void init_()
{
	program2 = glCreateProgram();

	/* Vertex */
	auto vertex = glCreateShader(GL_VERTEX_SHADER);
	{
		std::ifstream file("resources/shaders/visualize.vert.glsl");

		std::string src((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		const GLchar* src_ptr = src.c_str();

		glShaderSource(vertex, 1, &src_ptr, nullptr);
		glCompileShader(vertex);
	}
	glAttachShader(program2, vertex);

	/* Fragment */
	auto fragment = glCreateShader(GL_FRAGMENT_SHADER);
	{
		std::ifstream file("resources/shaders/visualize.frag.glsl");

		std::string src((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		const GLchar* src_ptr = src.c_str();

		glShaderSource(fragment, 1, &src_ptr, nullptr);
		glCompileShader(fragment);
	}
	glAttachShader(program2, fragment);

	glLinkProgram(program2);
}

void Voxelizer::Field::test_render(int what) const
{
	if (!init)
		init_();

	glUseProgram(program2);

	GLint location = glGetUniformLocation(program2, "u_camera");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(what == 0 ? this->y_proj() : (what == 1 ? this->x_proj() : this->z_proj())));

	this->render();

}

// ================================================================================================
// Voxelizer
// ================================================================================================

Voxelizer::Voxelizer()
{
	// Vertex
	Shader v_shader(GL_VERTEX_SHADER);
	v_shader.source_from_file("resources/shaders/voxelize.vert.glsl");
	if (!v_shader.compile())
	{
		std::cerr << v_shader.get_log() << std::endl;
		throw;
	}
	this->program.attach(v_shader);

	// Geometry
	Shader g_shader(GL_GEOMETRY_SHADER);
	g_shader.source_from_file("resources/shaders/voxelize.geom.glsl");
	if (!g_shader.compile())
	{
		std::cerr << g_shader.get_log() << std::endl;
		throw;
	}
	this->program.attach(g_shader);

	// Fragment
	Shader f_shader(GL_FRAGMENT_SHADER);
	f_shader.source_from_file("resources/shaders/voxelize.frag.glsl");
	if (!f_shader.compile())
	{
		std::cerr << f_shader.get_log() << std::endl;
		throw;
	}
	this->program.attach(f_shader);

	// Program
	if (!this->program.link())
	{
		std::cerr << this->program.get_log() << std::endl;
		throw;
	}
}

std::shared_ptr<const Voxelizer::Volume> Voxelizer::voxelize(std::shared_ptr<const Field> field, unsigned int height, unsigned int resolution)
{
	unsigned int side = (height / field->size().y) * field->largest_side();
	unsigned int actual_side = side * resolution;

	glEnable(GL_TEXTURE_3D);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	glViewport(0, 0, actual_side, actual_side);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	this->program.use();

	// Transform
	glUniformMatrix4fv(this->program.get_uniform_location("u_transform"), 1, GL_FALSE, glm::value_ptr(field->transform()));

	// Projections
	glUniformMatrix4fv(this->program.get_uniform_location("u_x_ortho_projection"), 1, GL_FALSE, glm::value_ptr(field->x_proj()));
	glUniformMatrix4fv(this->program.get_uniform_location("u_y_ortho_projection"), 1, GL_FALSE, glm::value_ptr(field->y_proj()));
	glUniformMatrix4fv(this->program.get_uniform_location("u_z_ortho_projection"), 1, GL_FALSE, glm::value_ptr(field->z_proj()));

	// Voxel Size
	glUniform1f(this->program.get_uniform_location("u_voxel_size"), actual_side);

	// Voxel
	auto volume = std::make_shared<Volume>(side, resolution);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_3D, volume->texture3d);
	glBindImageTexture(5, volume->texture3d, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);

	field->render();

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT); // ?

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	return volume;
}

// ================================================================================================
// Volume
// ================================================================================================

Voxelizer::Volume::Volume(unsigned int side, unsigned int resolution) : side(side), resolution(resolution)
{
	glGenTextures(1, &this->texture3d);
	glBindTexture(GL_TEXTURE_3D, this->texture3d);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	unsigned actual_side = side * resolution;

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, actual_side, actual_side, actual_side, 0, GL_RGBA, GL_FLOAT, NULL);
	glClearTexImage(this->texture3d, 0, GL_RGBA, GL_FLOAT, new float[4]{ 0, 0, 0, 0 });
}

Voxelizer::Volume::~Volume()
{
	glDeleteTextures(1, &this->texture3d);
}
