#pragma once

#include <memory>

#include <glm/glm.hpp>

// ================================================================================================

struct Field
{
	// The min/max points of the field.
	glm::vec3 min, max;

	// The size of the field, obtained using max - min.
	const glm::vec3 size;

	// The largest side of the field.
	const float largest_side;

	// Gets the matrix that if applied to the filed will bring it to the origin (0, 0, 0).
	glm::mat4 get_pivot() const;

	// Gets the matrix used to project the field on the X, Y and Z axes.
	glm::mat4 get_x_ortho() const, get_y_ortho() const, get_z_ortho() const;

	Field(glm::vec3 min, glm::vec3 max);

	virtual void render() const = 0;
};

