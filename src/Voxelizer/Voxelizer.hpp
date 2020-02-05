#pragma once

#include <memory>

#include "gl.hpp"

#include <glm/glm.hpp>
#include <GL/glew.h>


struct Volume
{
	GLuint resolution;

	GLsizei count;
	ShaderStorageBuffer data;

	Volume();
};

struct Voxelizer
{
	struct Field
	{
		glm::vec3 min;
		glm::vec3 max;

		glm::vec3 size() const;

		float largest_side() const;

		glm::mat4 transform() const;

		glm::mat4 x_proj() const;
		glm::mat4 y_proj() const;
		glm::mat4 z_proj() const;

		virtual void render() const = 0;
	};

	Program program;

	Voxelizer();

	GLuint sort(std::shared_ptr<const Volume> volume);

	std::shared_ptr<const Volume> voxelize(std::shared_ptr<const Field> field, GLuint height, GLuint resolution);
};

