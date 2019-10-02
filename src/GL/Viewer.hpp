#pragma once

#include "Camera.hpp"

#include <GLFW/glfw3.h>

class Viewer
{
private:
	tdogl::Camera camera;

	float movement_speed;
	float rotation_speed;

	double last_cursor_x, last_cursor_y;

	void process_movement(GLFWwindow* window, float delta);
	void process_rotation(GLFWwindow* window, float delta);

public:
	Viewer();

	tdogl::Camera get_camera();

	void on_update(GLFWwindow* window, float delta);
};
