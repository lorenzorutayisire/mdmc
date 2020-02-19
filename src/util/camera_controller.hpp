#pragma once

#include <GLFW/glfw3.h>

void process_camera_movement(GLFWwindow* window, const Camera& camera, float speed)
{
	if (glfwGetKey(window, GLFW_KEY_W))
		camera.offsetPosition(camera.forward() * speed);

	if (glfwGetKey(window, GLFW_KEY_S))
		camera.offsetPosition(camera.forward() * -speed);

	if (glfwGetKey(window, GLFW_KEY_A))
		camera.offsetPosition(camera.right() * -speed);

	if (glfwGetKey(window, GLFW_KEY_D))
		camera.offsetPosition(camera.right() * speed);

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
		camera.offsetPosition(camera.up() * -speed);

	if (glfwGetKey(window, GLFW_KEY_SPACE))
		camera.offsetPosition(camera.up() * speed);
}

void process_camera_rotation(GLFWwindow* window, const Camera& camera, float speed)
{
	double cursor_x, cursor_y;
	glfwGetCursorPos(window, &cursor_x, &cursor_y);

	if (cursor_x != this->last_cursor_x || cursor_y != this->last_cursor_y)
	{
		const double sensibility = 0.01;

		double offset_x = cursor_x - this->last_cursor_x;
		double offset_y = cursor_y - this->last_cursor_y;

		this->camera.offsetOrientation(offset_y * sensibility, offset_x * sensibility);

		this->last_cursor_x = cursor_x;
		this->last_cursor_y = cursor_y;
	}
}
