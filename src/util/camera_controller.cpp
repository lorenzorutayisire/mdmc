#include "camera_controller.hpp"

#include <imgui.h>

CameraController::CameraController() :
	last_cursor_x(0),
	last_cursor_y(0)
{}

bool CameraController::process_camera_movement(GLFWwindow* window, tdogl::Camera& camera, float speed)
{
	auto old_position = camera.position();

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

	return old_position != camera.position();
}

bool CameraController::process_camera_rotation(GLFWwindow* window, tdogl::Camera& camera, float speed)
{

	// If any ImGui item is hovered, we don't need to update the camera in any way.
	if (ImGui::IsAnyItemHovered())
		return false;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		this->last_cursor_x = -1;
		this->last_cursor_y = -1;

	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		double cursor_x, cursor_y;
		glfwGetCursorPos(window, &cursor_x, &cursor_y);
   		if (cursor_x != this->last_cursor_x || cursor_y != this->last_cursor_y)
		{
			if (this->last_cursor_x >= 0 && this->last_cursor_y >= 0)
			{
				double offset_x = cursor_x - this->last_cursor_x;
				double offset_y = cursor_y - this->last_cursor_y;

				camera.offsetOrientation(offset_y * speed, offset_x * speed);

				this->last_cursor_x = cursor_x;
				this->last_cursor_y = cursor_y;
			}
			else
			{
				int width, height;
				glfwGetWindowSize(window, &width, &height);
				glfwSetCursorPos(window, width / 2, height / 2);

				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

				this->last_cursor_x = width / 2;
				this->last_cursor_y = height / 2;
			}


			return true;
		}
	}

	return false;
}

