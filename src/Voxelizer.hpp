#pragma once

#include <memory>

#include "GL/Program.hpp"
#include "GL/Scene.hpp"

#include <GLFW/glfw3.h>

class Voxelizer
{
private:
	std::shared_ptr<Scene> scene;
	uint16_t width, height, depth;

	Program program;
	GLuint voxel; // texture3d

public:
	Voxelizer(Scene& scene, uint16_t height);
	~Voxelizer();

	inline uint16_t get_width() { return this->width; }
	inline uint16_t get_height() { return this->height; }
	inline uint16_t get_depth() { return this->depth; }
	inline uint16_t get_size() { return this->width * this->height * this->depth; }

	void voxelize(GLFWwindow* window);

	inline GLuint get_voxel() { return voxel; }
};
