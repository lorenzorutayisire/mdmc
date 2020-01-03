#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "Voxelizer/Voxelizer.hpp"

namespace mdmc
{
	struct Chunk : public Voxelizer::Field
	{
		std::shared_ptr<const Voxelizer::Field> field;

		Chunk(const std::shared_ptr<const Voxelizer::Field>& field, glm::vec3 from, float side);

		void render() const override;
	};
}
