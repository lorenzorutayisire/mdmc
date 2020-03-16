#pragma once

#include "util/gl.hpp"
#include "minecraft_world.hpp"

#include <glm/glm.hpp>

class MinecraftWorldRenderer
{
private:
	Program program;

public:
	MinecraftWorldRenderer();

	void render(const glm::mat4& camera, const glm::mat4& transform, const std::shared_ptr<MinecraftWorld>& world, const glm::vec4& tint_color);
};
