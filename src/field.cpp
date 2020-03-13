#include "field.hpp"

#include <glm/gtc/matrix_transform.hpp>

Field::Field(glm::vec3 min, glm::vec3 max) :
	min(min),
	max(max)
{}

glm::vec3 Field::size() const
{
	return this->max - this->min;
}

float Field::largest_side() const
{
	auto size = this->size();
	return glm::max(size.x, glm::max(size.y, size.z));
}

glm::mat4 Field::normalize_transf() const
{
	glm::mat4 res = glm::mat4(1.0);
	res = glm::scale(res, glm::vec3(1 / this->largest_side()));
	res = glm::translate(res, -this->min);
	return res;
}

void Field::ortho_proj(glm::mat4& x_proj, glm::mat4& y_proj, glm::mat4& z_proj) const
{
	glm::mat4 ortho = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 2.0f);

	x_proj = ortho * glm::lookAt(glm::vec3(-1, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	y_proj = ortho * glm::lookAt(glm::vec3(0, -1, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
	z_proj = ortho * glm::lookAt(glm::vec3(0,  0, 2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}
