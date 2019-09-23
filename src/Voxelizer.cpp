#include "Voxelizer.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

void genAtomicBuffer(int num, unsigned int& buffer)
{
	GLuint initVal = 0;

	if (buffer)
		glDeleteBuffers(1, &buffer);
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, buffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), &initVal, GL_STATIC_DRAW);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

}

int genLinearBuffer(int size, GLenum format, GLuint* tex, GLuint* tbo)
{
	GLenum err;

	// Texture buffer.
	if ((*tbo) > 0)
		glDeleteBuffers(1, tbo);  //delete previously created tbo

	glGenBuffers(1, tbo);
	glBindBuffer(GL_TEXTURE_BUFFER, *tbo);
	glBufferData(GL_TEXTURE_BUFFER, size, 0, GL_STATIC_DRAW);
	err = glGetError();

	// Texture bound to the previous buffer.
	if ((*tex) > 0)
		glDeleteTextures(1, tex); //delete previously created texture

	glGenTextures(1, tex);
	glBindTexture(GL_TEXTURE_BUFFER, *tex);
	glTexBuffer(GL_TEXTURE_BUFFER, format, *tbo);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	err = glGetError();
	if (err > 0)
		std::cout << glewGetErrorString(err) << std::endl;
	return err;
}

Voxelizer::Voxelizer(uint16_t width, uint16_t height, uint16_t depth) :
	width(width),
	height(height),
	depth(depth),
	size(width * height * depth)
{
	GLenum error;

	/* Voxel image */
	GLuint tbo;

	// TBO
	glGenBuffers(1, &tbo);
	glBindBuffer(GL_TEXTURE_BUFFER, tbo);
	glBufferData(GL_TEXTURE_BUFFER, this->size * 8 * 4, 0, GL_STATIC_DRAW);
	error = glGetError();
	if (error)
	{
		std::cout << glewGetErrorString(error) << std::endl;
		throw;
	}

	// Texture
	glGenTextures(1, &voxel);
	glBindTexture(GL_TEXTURE_BUFFER, voxel);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA8, voxel);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	/* Program */

	// Vertex
	Shader v_shader(GL_VERTEX_SHADER);
	v_shader.source_from_file("resources/shaders/voxelize.vert.glsl");
	if (v_shader.compile())
	{
		std::cerr << v_shader.get_compile_log() << std::endl;
		throw;
	}
	this->program.attach(v_shader);

	// Geometry
	Shader g_shader(GL_GEOMETRY_SHADER);
	g_shader.source_from_file("resources/shaders/voxelize.geom.glsl");
	if (g_shader.compile())
	{
		std::cerr << g_shader.get_compile_log() << std::endl;
		throw;
	}
	this->program.attach(g_shader);

	// Fragment
	Shader f_shader(GL_FRAGMENT_SHADER);
	f_shader.source_from_file("resources/shaders/voxelize.frag.glsl");
	if (f_shader.compile())
	{
		std::cerr << f_shader.get_compile_log() << std::endl;
		throw;
	}
	this->program.attach(f_shader);

	this->program.link();
}

Voxelizer::~Voxelizer()
{
}

void Voxelizer::voxelize(Scene& scene)
{
	/* OpenGL setup */

	glViewport(0, 0, this->width, this->height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// CULL_FACE must be disabled because we want vertices outside the screen.
	glDisable(GL_CULL_FACE);

	glDisable(GL_DEPTH_TEST);

	// Disables writing on framebuffer of all RGBA components, we store the result on a 3d image.
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	this->program.use();

	/* Geometry Shader */

	glm::mat4 ortho = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 2.0f - 1.0f, 3.0f);

	// Ortho-projection on the +X axis.
	glm::mat4 mvp_x = ortho * glm::lookAt(glm::vec3(2, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(this->program.get_uniform_location("u_MVPx"), 1, GL_FALSE, glm::value_ptr(mvp_x));

	// Ortho-projection on the +Y axis.
	glm::mat4 mvp_y = ortho * glm::lookAt(glm::vec3(0, 2, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));
	glUniformMatrix4fv(this->program.get_uniform_location("u_MVPy"), 1, GL_FALSE, glm::value_ptr(mvp_y));

	// Ortho-projection on the +Z axis.
	glm::mat4 mvp_z = ortho * glm::lookAt(glm::vec3(0, 0, 2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(this->program.get_uniform_location("u_MVPz"), 1, GL_FALSE, glm::value_ptr(mvp_z));

	// The width of the voxel space.
	glUniform1i(this->program.get_uniform_location("u_width"), this->width);

	// The height of the voxel space.
	glUniform1i(this->program.get_uniform_location("u_height"), this->height);

	/* Fragment Shader */

	glUniform3i(this->program.get_uniform_location("u_voxel_size"), this->width, this->height, this->depth);
	glBindImageTexture(0, voxel, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);

	for (Mesh mesh : scene.get_meshes())
	{
		glBindTexture(GL_TEXTURE_2D, mesh.get_texture());
		glUniform4f(this->program.get_uniform_location("u_color"), mesh.get_color().r, mesh.get_color().g, mesh.get_color().b, mesh.get_color().a);

		glBindVertexArray(mesh.get_vao());
		glDrawElements(GL_TRIANGLES, mesh.get_elements_count(), GL_UNSIGNED_INT, NULL);
	}

	// scene.render();
}

