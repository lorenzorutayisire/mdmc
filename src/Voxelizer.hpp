#pragma once

#include "Program.hpp"

#include <GLFW/glfw3.h>
#include "Scene/Scene.hpp"

class Voxelizer
{
private:
	Program program;
	GLuint voxel;

public:
	Voxelizer();
	~Voxelizer();

	void voxelize(GLFWwindow* window, Scene& scene, uint16_t height);
};
