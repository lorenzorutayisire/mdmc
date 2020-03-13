#pragma once

#include <GLFW/glfw3.h>

#include "camera.hpp"

class CameraController
{
private:
	double last_cursor_x, last_cursor_y;

public:
	CameraController();

	bool process_camera_movement(GLFWwindow* window, tdogl::Camera& camera, float speed);
	bool process_camera_rotation(GLFWwindow* window, tdogl::Camera& camera, float speed);
};
