#pragma once

#include <GL/glew.h>
#include <memory>

#include "Voxelizer/Voxelizer.hpp"

using Volume = Voxelizer::Volume;

namespace Minecraft
{
	struct Schematic
	{
		unsigned int side;
		GLuint texture3d;

		Schematic(unsigned int side);
		~Schematic();
	};

	struct BlocksPalette
	{
		unsigned int step, count;
		std::shared_ptr<const Volume> volume;

		BlocksPalette(unsigned int step, unsigned int count, std::shared_ptr<const Volume> volume);
	};

	struct Mapper
	{
		GLuint program;

		Mapper();
		~Mapper();

		struct Result
		{
			std::shared_ptr<const Schematic> schematic;
			std::shared_ptr<const Volume> mapped_model; // Debug

			Result(unsigned int side, unsigned int resolution);
		};

		Result map(std::shared_ptr<const Volume> model, const BlocksPalette& blocks);
	};
}
