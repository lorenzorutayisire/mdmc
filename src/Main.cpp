
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>

#include "PhaseManager.hpp"
#include "Routine/LoadingPhase.hpp"

#include "Minecraft/TextureAsset.hpp"

void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}


int main(int argc, char** argv)
{
	if (argc != (1 + 3)) // The first is the name of the exec.
	{
		std::cerr << "Wrong arguments: <model_path> <height> <minecraft_version>" << std::endl;
		return 1;
	}

	/* Model path */
	const std::string model_path = argv[1];
	std::ifstream model_file(model_path);
	if (!model_file.good())
	{
		std::cerr << "Can't open file at: " << model_path << std::endl;
		return 2;
	}

	/* Height */
	const int raw_height = atoi(argv[2]);
	if (raw_height < 0 || raw_height > 256)
	{
		std::cerr << "A Minecraft schematic can't be taller than 256 blocks, negative or 0." << std::endl;
		return 3;
	}
	const uint16_t height = (uint16_t)raw_height;

	/* Minecraft version */
	const std::string minecraft_version = argv[3];
	std::ifstream minecraft_asset_file("resources/minecraft_assets/" + minecraft_version + ".bin");
	if (!minecraft_asset_file.good())
	{
		std::cerr << "Unsupported Minecraft version: " + minecraft_version + ".";
		return 4;
	}

	if (glfwInit() != GLFW_TRUE)
	{
		std::cerr << "Failed to initialize GLFW." << std::endl;
		throw;
	}
	std::cout << "GLFW initialized." << std::endl;

	GLFWwindow* window = glfwCreateWindow(512, 512, "Hello world", NULL, NULL);
	glfwMakeContextCurrent(window);

	//glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE); // We don't want the old OpenGL 

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW." << std::endl;
		throw;
	}
	std::cout << "GLEW initialized." << std::endl;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_TEXTURE_3D);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	glfwShowWindow(window);

	PhaseManager phase_manager(window);

	phase_manager.set_phase(new LoadingPhase(model_path.c_str()));

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0, 0, 0.5, 0);

		phase_manager.on_update(1.0f);
		phase_manager.on_render();

		// Closes the window when ESCAPE key is pressed.
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
}
