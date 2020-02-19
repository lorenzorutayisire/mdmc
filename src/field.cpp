#include "field.hpp"

#include <glm/gtc/matrix_transform.hpp>

// ================================================================================================

Field::Field(glm::vec3 min, glm::vec3 max) :
	min(min),
	max(max),
	size(max - min),
	largest_side(glm::max(size.x, glm::max(size.y, size.z)))
{}

glm::mat4 Field::get_pivot() const
{
	glm::mat4 transform = glm::mat4(1.0);
	transform = glm::scale(transform, glm::vec3(1 / this->largest_side));
	transform = glm::translate(transform, -this->min);
	return transform;
}

glm::mat4 Field::get_x_ortho() const
{
	glm::mat4 ortho = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 2.0f);
	return (ortho * glm::lookAt(glm::vec3(-1, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0))) * this->get_pivot();
}

glm::mat4 Field::get_y_ortho() const
{
	glm::mat4 ortho = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 2.0f);
	return ortho * glm::lookAt(glm::vec3(0, -1, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1)) * this->get_pivot();
}

glm::mat4 Field::get_z_ortho() const
{
	glm::mat4 ortho = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 2.0f);
	return ortho * glm::lookAt(glm::vec3(0, 0, 2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)) * this->get_pivot();
}
