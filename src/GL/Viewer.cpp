#include "Viewer.hpp"

Viewer::Viewer() :
	movement_speed(0.1),
	rotation_speed(1.0),

	last_cursor_x(0),
	last_cursor_y(0)
{
}

tdogl::Camera Viewer::get_camera()
{
	return camera;
}

void Viewer::process_movement(GLFWwindow* window, float delta)
{
	if (glfwGetKey(window, GLFW_KEY_W))
	{
		this->camera.offsetPosition(this->camera.forward() * this->movement_speed);
	}
	if (glfwGetKey(window, GLFW_KEY_S))
	{
		this->camera.offsetPosition(this->camera.forward() * -this->movement_speed);
	}
	if (glfwGetKey(window, GLFW_KEY_A))
	{
		this->camera.offsetPosition(this->camera.right() * -this->movement_speed);
	}
	if (glfwGetKey(window, GLFW_KEY_D))
	{
		this->camera.offsetPosition(this->camera.right() * this->movement_speed);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
	{
		this->camera.offsetPosition(this->camera.up() * -this->movement_speed);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE))
	{
		this->camera.offsetPosition(this->camera.up() * this->movement_speed);
	}
}

void Viewer::process_rotation(GLFWwindow* window, float delta)
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

void Viewer::on_update(GLFWwindow* window, float delta)
{
	this->process_movement(window, delta);
	this->process_rotation(window, delta);
}
