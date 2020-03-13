#pragma once

#include <memory>

#include <glm/glm.hpp>

// ================================================================================================

struct Field
{
	// The min/max points of the field.
	glm::vec3 min, max;

	glm::vec3 size() const;
	float largest_side() const;

	glm::mat4 normalize_transf() const;
	void ortho_proj(glm::mat4& x, glm::mat4& y, glm::mat4& z) const;

	Field(glm::vec3 min, glm::vec3 max);

	virtual void render() const = 0;
};

