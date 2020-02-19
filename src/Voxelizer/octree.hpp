#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "volume.hpp"

struct Octree
{
	const TextureBuffer texture_buffer;

	Octree(const TextureBuffer& texture_buffer);

};

struct OctreeBuilder
{
	Program program;

	OctreeBuilder();

	std::shared_ptr<const Octree> build(const Volume& volume);
};
