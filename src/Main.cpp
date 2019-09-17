
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
		throw;
	}

	GLFWwindow *window = glfwCreateWindow(512, 512, "Hello world", NULL, NULL);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		throw;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_TEXTURE_3D);

	glfwShowWindow(window);

	PhaseManager phaseManager(window);

	phaseManager.setPhase(new LoadScenePhase("resources/skull/12140_Skull_v3_L2.obj"));

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

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

	return 0;
}
