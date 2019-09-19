#include "VisualizeScenePhase.hpp"

#include "../Scene/Shader.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

const GLchar* VISUALIZE_VERTEX_SHADER = R"glsl(
	#version 450

	layout (location = 0) in vec3 position;
	layout (location = 1) in vec2 texCoords;

	uniform mat4 camera;

	out vec2 TexCoords;

	void main()
	{
		gl_Position = camera * vec4(position, 1.0);
		TexCoords = texCoords;
	}
)glsl";

const GLchar* VISUALIZE_FRAGMENT_SHADER = R"glsl(
	#version 450

	in vec2 TexCoords;

	layout(location = 2) uniform vec4 color;
	uniform sampler2D texture2d;

	void main()
	{
		gl_FragColor = color * texture(texture2d, TexCoords);
	}
)glsl";

VisualizeScenePhase::VisualizeScenePhase(Scene* scene) :
	scene(scene),
	program(0),
	lastCursorX(0),
	lastCursorY(0)
{
}

GLuint create_shader(GLenum type, const GLchar* source)
{
	GLuint shader;

	shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		char log[2048];
		glGetShaderInfoLog(shader, 2048, NULL, &log[0]);
		std::cerr << log << std::endl;
		throw;
	}

	return shader;
}

void VisualizeScenePhase::onEnable(PhaseManager* phaseManager)
{
	std::cout << "========================================" << std::endl;
	std::cout << "VisualizeScenePhase | onEnable" << std::endl;
	std::cout << "========================================" << std::endl;

	// Program
	this->program = glCreateProgram();

	GLuint shader;
	
	shader = create_shader(GL_VERTEX_SHADER, VISUALIZE_VERTEX_SHADER);
	glAttachShader(this->program, shader);

	shader = create_shader(GL_FRAGMENT_SHADER, VISUALIZE_FRAGMENT_SHADER);
	glAttachShader(this->program, shader);

	glLinkProgram(this->program);

	// Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int*)& isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

		std::cerr << "Could not link program." << std::endl;
		std::cout << "Log length: " << infoLog.size() << std::endl;
		std::cout << "Log: " << std::string(infoLog.begin(), infoLog.end()) << std::endl;

		throw;
	}

	/*
	GLint isLinked = 0;
	glGetProgramiv(this->program, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(this->program, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(this->program, maxLength, &maxLength, &infoLog[0]);

		std::cout << "Error while linking program: " << std::string(infoLog.begin(), infoLog.end()) << std::endl;

		// The program is useless now. So delete it.
		glDeleteProgram(this->program);

		// Provide the infolog in whatever manner you deem best.
		// Exit with failure.
		return;
	}
	*/

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

	if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
	{
		return;
	}

	// Camera Movement
	const float movement_sensibility = 0.01 * delta;

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

void VisualizeScenePhase::onRender(PhaseManager* phaseManager)
{
	GLint location;

	glUseProgram(this->program);

	// Transform
	location = glGetUniformLocation(this->program, "transform");
	if (location < 0)
	{
		//std::cerr << "Cannot get transform uniform location." << std::endl;
		//throw;
	}
	//glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

	// Camera
	location = glGetUniformLocation(this->program, "camera");
	if (location < 0)
	{
		std::cerr << "Cannot get camera uniform location." << std::endl;
		throw;
	}
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(this->camera.matrix()));

	this->scene->render();
}
