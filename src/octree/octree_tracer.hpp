#pragma once

#include <memory>

#include "octree.hpp"

#include "util/screen_quad.hpp"
#include "util/camera.hpp"

class OctreeRenderer
{
private:
	Program program;
	ScreenQuad screen_quad;

public:
	OctreeRenderer();

	void render(
		glm::uvec2 screen_size,
		glm::mat4 const& camera_projection,
		glm::mat4 const& camera_view,
		glm::vec3 const& camera_position,
		std::shared_ptr<Octree> const& octree
	);
};


