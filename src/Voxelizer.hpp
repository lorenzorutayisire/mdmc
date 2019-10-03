#pragma once

#include <memory>

#include "GL/Program.hpp"
#include "GL/Scene.hpp"

#include <GLFW/glfw3.h>

class Voxelizer
{
private:
	std::shared_ptr<Scene> scene;
	uint16_t side;

	glm::mat4 transform;
	glm::mat4 x_ortho_projection, y_ortho_projection, z_ortho_projection;

	Program program;
	GLuint voxel; // texture3d

public:
	Voxelizer(Scene& scene, uint16_t height);
	~Voxelizer();

	std::shared_ptr<Scene> get_scene() { return this->scene; }

	glm::mat4 get_transform() { return this->transform; }

	glm::mat4 get_x_ortho_projection() { return this->x_ortho_projection; }
	glm::mat4 get_y_ortho_projection() { return this->y_ortho_projection; }
	glm::mat4 get_z_ortho_projection() { return this->z_ortho_projection; }

	inline uint16_t get_side() { return this->side; }

	void voxelize();

	inline GLuint get_voxel() { return voxel; }
};
