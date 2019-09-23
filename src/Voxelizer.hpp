#pragma once

#include "Program.hpp"

#include "Scene/Scene.hpp"

class Voxelizer
{
private:
	uint16_t width, height, depth;
	uint32_t size;

	Program program;

	GLuint voxel;

public:
	Voxelizer(uint16_t width, uint16_t height, uint16_t depth);
	~Voxelizer();

	void voxelize(Scene& scene);
};
