
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
	/* ARGS */
	// Gets from the command-line the required arguments.

	if (argc != (1 + 3)) // The first is the name of the exec.
	{
		std::cerr << "Wrong arguments: <model_path> <height> <minecraft_version>" << std::endl;
		return 1;
	}

	// <model_path>
	const std::string model_path = argv[1];
	std::ifstream model_file(model_path);
	if (!model_file.good())
	{
		std::cerr << "Can't open file at: " << model_path << std::endl;
		return 2;
	}

	// <height>
	const int raw_height = atoi(argv[2]);

#ifndef UNLIMITED_HEIGHT
	if (raw_height > 256)
	{
		std::cerr << "A Minecraft schematic can't be taller than 256 blocks." << std::endl;
		return 3;
	}
#endif
	if (raw_height <= 0)
	{
		std::cerr << "A Minecraft schematic can't be negative or 0." << std::endl;
		return 3;
	}
	
	const uint16_t height = (uint16_t)raw_height;

	// <minecraft_version>
	const std::string minecraft_version = argv[3];
	std::ifstream minecraft_asset_file("resources/minecraft_assets/" + minecraft_version + ".bin", std::ios::binary);
	if (!minecraft_asset_file.good())
	{
		std::cerr << "Unsupported Minecraft version: " + minecraft_version + ".";
		return 4;
	}

	/* INIT */
	// Initializes GLFW & GLEW stuff.

	if (glfwInit() != GLFW_TRUE)
	{
		std::cerr << "GLFW failed to initialize." << std::endl;
		return 5;
	}

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // Must be placed before window creation.

	GLFWwindow* window = glfwCreateWindow(712, 712, "MDMC", NULL, NULL);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // OpenGL 4.2 required to run Image load/store extension.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE); // To make MacOS happy, should not be needed.
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL. 
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "GLEW failed to initialize." << std::endl;
		return 6;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_TEXTURE_3D);

	glEnable(GL_DEBUG_OUTPUT); // Enables OpenGL warning & error logging.
	glDebugMessageCallback(MessageCallback, 0);

	glfwShowWindow(window); // Now the window can be shown.

	/* LOADING */

	PhaseManager phase_manager(window);

	phase_manager.set_phase(new LoadingPhase(model_path.c_str(), height, minecraft_asset_file));
	
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
