#pragma once

#include <map>
#include <unordered_set>
#include <functional>

#include <GLFW/glfw3.h>

class GlfwCallbackHandler
{
private:
	GLFWwindow* window;

	using KeyCallback = std::function<void()>;
	std::map<std::pair<int, int>, KeyCallback> key_callbacks;

	static void handle_key(GLFWwindow* window, int key, int scan_code, int action, int mods)
	{
		auto handle = static_cast<GlfwCallbackHandler*>(glfwGetWindowUserPointer(window));
		auto id = std::make_pair(key, action);
		if (handle->key_callbacks.find(id) != handle->key_callbacks.end())
			handle->key_callbacks.at(id)();
	}

public:
	GlfwCallbackHandler(GLFWwindow* window) :
		window(window)
	{
		// From now on, this window should have only this user pointer set.
		glfwSetWindowUserPointer(window, this);

		glfwSetKeyCallback(window, GlfwCallbackHandler::handle_key);
	}

	~GlfwCallbackHandler()
	{
		glfwSetKeyCallback(this->window, nullptr);

		glfwSetWindowUserPointer(this->window, nullptr);
	}

	GlfwCallbackHandler(GlfwCallbackHandler const&) = delete;
	GlfwCallbackHandler(GlfwCallbackHandler const&&) = delete;

	void sub_key_callback(int key, int action, KeyCallback const& callback)
	{
		auto id = std::make_pair(key, action);
		key_callbacks.insert(std::make_pair(id, callback));
	}

	void rem_key_callback(int key, int action)
	{
		auto id = std::make_pair(key, action);
		key_callbacks.erase(id);
	}
};
