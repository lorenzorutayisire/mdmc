#pragma once

#include "util/camera.hpp"
#include "util/gl.hpp"

#include "scene.hpp"

class SceneRenderer
{
private:
	Program program;

public:
	SceneRenderer();

	void render(const glm::mat4& camera, const glm::mat4& transform, const std::shared_ptr<Scene>& scene, Material::Type view_type);
};
