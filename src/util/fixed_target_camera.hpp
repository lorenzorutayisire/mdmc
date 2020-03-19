#pragma once

#include <functional>
#include <tuple>

#include <glm/glm.hpp>

class FixedTargetCamera
{
private:
	glm::vec2 rotation;
	float zoom;

public:
	glm::vec3 target;

	FixedTargetCamera(glm::vec3 const& target);

	glm::vec3 get_position();

	glm::vec2 const& get_rotation();
	void offset_rotation(glm::vec2 const& offset);

	float get_zoom();
	void offset_zoom(float offset);

	glm::mat4 get_matrix();
};
