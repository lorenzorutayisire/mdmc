#include "VisualizeScenePhase.hpp"

#include "../Scene/Shader.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

const GLchar* VISUALIZE_VERTEX_SHADER = R"glsl(
	#version 450

	layout (location = 0) in vec3 position;
	layout (location = 1) in vec2 texCoords;

	out vec2 TexCoords;

	layout (location = 0) uniform mat4 transform;
	layout (location = 1) uniform mat4 camera;

	void main() {
		gl_Position = camera * transform * vec4(position, 1.0);
		TexCoords = texCoords;
	}
)glsl";

const GLchar* VISUALIZE_FRAGMENT_SHADER = R"glsl(
	#version 450

	in vec2 TexCoords;

	uniform sampler2D texture2d;

	void main() {
		gl_FragColor = texture(texture2d, TexCoords);
	}
)glsl";

VisualizeScenePhase::VisualizeScenePhase(Scene* scene) :
	scene(scene),
	program(0),
	lastCursorX(0),
	lastCursorY(0)
{
}

void VisualizeScenePhase::onEnable(PhaseManager* phaseManager)
{
	std::cout << "========================================" << std::endl;
	std::cout << "VisualizeScenePhase | onEnable" << std::endl;
	std::cout << "========================================" << std::endl;

	// Program
	this->program = glCreateProgram();

	GLuint shader;
	
	std::cout << "Loading the vertex shader..." << std::endl;
	shader = Shader::loadShader(GL_VERTEX_SHADER, VISUALIZE_VERTEX_SHADER);
	glAttachShader(this->program, shader);

	std::cout << "Loading the fragment shader..." << std::endl;
	shader = Shader::loadShader(GL_FRAGMENT_SHADER, VISUALIZE_FRAGMENT_SHADER);
	glAttachShader(this->program, shader);

	glLinkProgram(this->program);

	std::cout << "Program ready to visualize your 3d scene." << std::endl;

	// Camera
	glfwSetInputMode(phaseManager->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void VisualizeScenePhase::onDisable(PhaseManager* phaseManager)
{
	glDeleteProgram(this->program);

	std::cout << "========================================" << std::endl;
	std::cout << "VisualizeScenePhase | onDisable" << std::endl;
	std::cout << "========================================" << std::endl;
}

void VisualizeScenePhase::onUpdate(PhaseManager* phaseManager, float delta)
{
	GLFWwindow* window = phaseManager->getWindow();

	// Camera Movement
	if (glfwGetKey(window, GLFW_KEY_W))
	{
		this->camera.offsetPosition(this->camera.forward() * delta);
	}
	if (glfwGetKey(window, GLFW_KEY_S))
	{
		this->camera.offsetPosition(this->camera.forward() * -delta);
	}
	if (glfwGetKey(window, GLFW_KEY_A))
	{
		this->camera.offsetPosition(this->camera.right() * -delta);
	}
	if (glfwGetKey(window, GLFW_KEY_D))
	{
		this->camera.offsetPosition(this->camera.right() * delta);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
	{
		this->camera.offsetPosition(this->camera.up() * -delta);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE))
	{
		this->camera.offsetPosition(this->camera.up() * delta);
	}

	// Camera Rotation
	double cursorX, cursorY;
	glfwGetCursorPos(window, &cursorX, &cursorY);
	
	if (cursorX != this->lastCursorX || cursorY != this->lastCursorY)
	{
		double offsetX = cursorX - this->lastCursorX;
		double offsetY = this->lastCursorY - cursorY;

		this->camera.offsetOrientation(offsetX, offsetY);

		this->lastCursorX = cursorX;
		this->lastCursorY = cursorY;
	}
}

void VisualizeScenePhase::onRender(PhaseManager* phaseManager)
{
	glUseProgram(this->program);

	glClearColor(0.3f, 1.0f, 0.3f, 1.0f);

	// Transform
	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(glm::mat4()));

	// Camera
	glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(camera.matrix()));

	this->scene->render();
}
