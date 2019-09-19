
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>

#include "Phases/PhaseManager.hpp"
#include "Phases/LoadScenePhase.hpp"


int main()
{
	if (glfwInit() != GLFW_TRUE)
	{
		std::cerr << "Failed to initialize GLFW." << std::endl;
		throw;
	}
	std::cout << "GLFW initialized." << std::endl;

	GLFWwindow *window = glfwCreateWindow(512, 512, "Hello world", NULL, NULL);
	glfwMakeContextCurrent(window);

	//glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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

	glfwShowWindow(window);

	PhaseManager phaseManager(window);

	phaseManager.setPhase(new LoadScenePhase("resources/buildings/building.obj"));

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0, 0, 0, 0);

		phaseManager.onUpdate(1.0f);
		phaseManager.onRender();

		// Closes the window when ESCAPE key is pressed.
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}
