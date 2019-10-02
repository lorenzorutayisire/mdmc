#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Scene.hpp"

class VoxelStage
{
private:

public:
	VoxelStage(Scene* scene)
	{

	}

	glm::mat4 get_transform() { return this->transform; }
	
	glm::mat4 get_x_ortho_projection() { return this->x_ortho_projection; }
	glm::mat4 get_y_ortho_projection() { return this->y_ortho_projection; }
	glm::mat4 get_z_ortho_projection() { return this->z_ortho_projection; }
};
