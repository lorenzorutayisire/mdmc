#include "fixed_target_camera.hpp"

#include <glm/gtx/transform.hpp>

FixedTargetCamera::FixedTargetCamera(glm::vec3 const& target) :
	target(target),
	rotation(glm::vec2(0)),
	zoom(1)
{}

glm::vec3 FixedTargetCamera::get_position()
{
	glm::vec3 position;
	position.x = this->zoom * glm::cos(rotation.x) * glm::cos(rotation.y);
	position.y = this->zoom * glm::cos(rotation.x) * glm::sin(rotation.y);
	position.z = this->zoom * glm::sin(rotation.x);
	return position + this->target;
}

float FixedTargetCamera::get_zoom()
{
	return this->zoom;
}

void FixedTargetCamera::offset_zoom(float offset)
{
	this->zoom += offset;
	if (this->zoom <= 0.1f) this->zoom = 0.1f; 
}


glm::vec2 const& FixedTargetCamera::get_rotation()
{
	return this->rotation;
}

void FixedTargetCamera::offset_rotation(glm::vec2 const& offset)
{
	this->rotation += offset;

	constexpr float radius = glm::pi<float>() / 4; // 45°
	if (this->rotation.y > radius) this->rotation.y = radius;
	if (this->rotation.y < -radius) this->rotation.y = -radius;
}

glm::mat4 FixedTargetCamera::get_matrix()
{ 
	return glm::lookAt(this->get_position(), this->target, glm::vec3(0, 1, 0));
}
