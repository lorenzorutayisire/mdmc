#include "Mapper.hpp"

#include <fstream>

using namespace Minecraft;

// ================================================================================================
// Schematic
// ================================================================================================

Schematic::Schematic(unsigned int side) : side(side)
{
	glGenTextures(1, &this->texture3d);
	glBindTexture(GL_TEXTURE_3D, this->texture3d);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32UI, side, side, side);
}

Schematic::~Schematic()
{
	glDeleteTextures(1, &this->texture3d);
}

// ================================================================================================
// BlocksPalette
// ================================================================================================

BlocksPalette::BlocksPalette(unsigned int step, unsigned int count, std::shared_ptr<const Volume> volume) : step(step), count(count), volume(volume)
{
}

// ================================================================================================
// Mapper
// ================================================================================================

Mapper::Mapper()
{
	this->program = glCreateProgram();

	auto compute = glCreateShader(GL_COMPUTE_SHADER);
	{
		std::ifstream file("resources/shaders/mc_mapper.comp.glsl");

		std::string src((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		const GLchar* src_ptr = src.c_str();

		glShaderSource(compute, 1, &src_ptr, nullptr);
		glCompileShader(compute);
	}
	glAttachShader(this->program, compute);

	glLinkProgram(this->program);

	glDeleteShader(compute);
}

Mapper::~Mapper()
{
	glDeleteProgram(this->program);
}

Mapper::Result::Result(unsigned int side, unsigned int resolution)
{
	this->schematic = std::make_shared<Schematic>(side);
	this->mapped_model = std::make_shared<Volume>(glm::uvec3(side, side, side));
}

Mapper::Result Mapper::map(std::shared_ptr<const Volume> model, const BlocksPalette& blocks)
{
	Mapper::Result result(model->size.x, model->size.y); // size, resolution

	glUseProgram(this->program);

	glUniform1ui(0, 2 /*resolution*/);
	glUniform1ui(1, blocks.count);

	/* model */
	glActiveTexture(GL_TEXTURE0);
	glBindImageTexture(0, model->texture3d, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);

	/* blocks */
	glActiveTexture(GL_TEXTURE1);
	glBindImageTexture(1, blocks.volume->texture3d, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);

	/* mapped_model */
	glActiveTexture(GL_TEXTURE2);
	glBindImageTexture(2, result.mapped_model->texture3d, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);

	/* mc_schematic */
	glActiveTexture(GL_TEXTURE3);
	glBindImageTexture(3, result.schematic->texture3d, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32UI);

	glDispatchCompute(model->size.x, model->size.y, model->size.z);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	return result;
}

