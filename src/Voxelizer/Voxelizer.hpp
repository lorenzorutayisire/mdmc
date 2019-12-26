#pragma once

#include <memory>

#include "GL/Program.hpp"

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <GL/glew.h>

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

	struct Volume
	{
		unsigned int side;
		GLuint texture3d;

		Volume();
		~Volume();
	};

	Program program;

	Voxelizer();

	std::shared_ptr<const Volume> voxelize(std::shared_ptr<const Field> field, unsigned int height);
};

