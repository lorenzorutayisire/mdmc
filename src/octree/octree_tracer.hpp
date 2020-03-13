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

	void render(glm::uvec2 screen_size, const tdogl::Camera& camera, const std::shared_ptr<Octree>& octree);
};


