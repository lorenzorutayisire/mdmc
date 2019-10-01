#include "VoxelizeScenePhase.hpp"

#include <GLFW/glfw3.h>
#include <iostream>

VoxelizeScenePhase::VoxelizeScenePhase(Scene* scene) :
	lastCursorX(0),
	lastCursorY(0)
{
	this->scene = scene;
}

void VoxelizeScenePhase::onEnable(PhaseManager* phaseManager)
{
	this->voxelizer = std::make_unique<Voxelizer>(Voxelizer(*this->scene, 32));
	this->voxelizer->voxelize(phaseManager->getWindow());
	std::cout << "Voxelization done" << std::endl;

	// Restore window viewport.
	int width, height;
	glfwGetWindowSize(phaseManager->getWindow(), &width, &height);
	glViewport(0, 0, width, height);


	// ====================================================== VBO

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	std::vector<GLfloat> vertices;
	glm::vec3 size = this->scene->get_size();

	// X planes
	for (GLfloat x = 0.5; x < size.x; x++)
	{
		vertices.push_back(x);
		vertices.push_back(0);
		vertices.push_back(0);

		vertices.push_back(x);
		vertices.push_back(size.y);
		vertices.push_back(0);

		vertices.push_back(x);
		vertices.push_back(size.y);
		vertices.push_back(size.z);

		vertices.push_back(x);
		vertices.push_back(0);
		vertices.push_back(size.z);
	}

	// Y planes
	for (GLfloat y = 0.5; y <= size.y; y++)
	{
		vertices.push_back(0);
		vertices.push_back(y);
		vertices.push_back(0);

		vertices.push_back(size.x);
		vertices.push_back(y);
		vertices.push_back(0);

		vertices.push_back(size.x);
		vertices.push_back(y);
		vertices.push_back(size.z);

		vertices.push_back(0);
		vertices.push_back(y);
		vertices.push_back(size.z);
	}

	// Z planes
	for (GLfloat z = 0.5; z < size.z; z++)
	{
		vertices.push_back(0);
		vertices.push_back(0);
		vertices.push_back(z);

		vertices.push_back(size.x);
		vertices.push_back(0);
		vertices.push_back(z);

		vertices.push_back(size.x);
		vertices.push_back(size.y);
		vertices.push_back(z);

		vertices.push_back(0);
		vertices.push_back(size.y);
		vertices.push_back(z);
	}

	vertices_count = vertices.size();
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

	// ====================================================== Program
	// Vertex
	Shader v_shader(GL_VERTEX_SHADER);
	v_shader.source_from_file("resources/shaders/render_voxel_plane.vert.glsl");
	if (!v_shader.compile())
	{
		std::cerr << v_shader.get_log() << std::endl;
		throw;
	}
	this->program.attach(v_shader);

	/*
	// Geometry
	Shader g_shader(GL_GEOMETRY_SHADER);
	g_shader.source_from_file("resources/shaders/render_voxel.geom.glsl");
	if (!g_shader.compile())
	{
		std::cerr << g_shader.get_log() << std::endl;
		throw;
	}
	this->program.attach(g_shader);
	*/

	// Fragment
	Shader f_shader(GL_FRAGMENT_SHADER);
	f_shader.source_from_file("resources/shaders/render_voxel_plane.frag.glsl");
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

	// Enables alpha test
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_BLEND);

	// Keep triangles with anti-clockwise vertices order
	glDisable(GL_CULL_FACE);
}

void VoxelizeScenePhase::onUpdate(PhaseManager* phaseManager, float delta)
{
	GLFWwindow* window = phaseManager->getWindow();

	if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
	{
		return;
	}

	// Camera Movement
	const float movement_sensibility = 0.1 * delta;

	if (glfwGetKey(window, GLFW_KEY_W))
	{
		this->camera.offsetPosition(this->camera.forward() * movement_sensibility);
	}
	if (glfwGetKey(window, GLFW_KEY_S))
	{
		this->camera.offsetPosition(this->camera.forward() * -movement_sensibility);
	}
	if (glfwGetKey(window, GLFW_KEY_A))
	{
		this->camera.offsetPosition(this->camera.right() * -movement_sensibility);
	}
	if (glfwGetKey(window, GLFW_KEY_D))
	{
		this->camera.offsetPosition(this->camera.right() * movement_sensibility);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
	{
		this->camera.offsetPosition(this->camera.up() * -movement_sensibility);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE))
	{
		this->camera.offsetPosition(this->camera.up() * movement_sensibility);
	}
	//std::cout << "Camera position: " << this->camera.position().x << " " << this->camera.position().y << " " << this->camera.position().z << std::endl;

	// Camera Rotation
	double cursorX, cursorY;
	glfwGetCursorPos(window, &cursorX, &cursorY);

	if (cursorX != this->lastCursorX || cursorY != this->lastCursorY)
	{
		const double sensibility = 0.01;

		double offsetX = cursorX - this->lastCursorX;
		double offsetY = cursorY - this->lastCursorY;

		this->camera.offsetOrientation(offsetY * sensibility, offsetX * sensibility);

		this->lastCursorX = cursorX;
		this->lastCursorY = cursorY;
	}
}

void VoxelizeScenePhase::onRender(PhaseManager* phaseManager)
{
	this->program.use();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1, 1, 1, 0);

	// Transform
	glm::mat4 transform = glm::mat4(1.0);
	glUniformMatrix4fv(this->program.get_uniform_location("u_transform"), 1, GL_FALSE, glm::value_ptr(transform));

	// Camera
	glm::mat4 camera = this->camera.matrix();
	glUniformMatrix4fv(this->program.get_uniform_location("u_camera"), 1, GL_FALSE, glm::value_ptr(camera));

	// Voxel Size
	glUniform3f(this->program.get_uniform_location("u_voxel_size"), this->voxelizer->get_width(), this->voxelizer->get_height(), this->voxelizer->get_depth());

	// Voxel
	GLuint voxel = this->voxelizer->get_voxel();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, voxel);

	// ====================================================== VBO
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	glDrawArrays(GL_QUADS, 0, vertices_count);
}
