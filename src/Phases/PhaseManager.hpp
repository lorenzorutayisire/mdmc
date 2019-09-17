#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <memory>

class Phase;

class PhaseManager
{
private:
	GLFWwindow* window;

	std::shared_ptr<Phase> phase;

public:
	PhaseManager(GLFWwindow* window);
	~PhaseManager();

	GLFWwindow* getWindow();
	void setPhase(Phase* phase);

	void onUpdate(float delta);
	void onRender();
};
