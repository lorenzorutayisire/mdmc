#include "minecraft_assets_phase.hpp"

#include <filesystem>
#include <iostream>
#include <thread>
#include <chrono>
#include <iostream>

#include <imgui.h>

#include <glm/gtx/transform.hpp>

#include "minecraft/assets/minecraft_assets.hpp"

namespace fs = std::filesystem;

glm::mat4 get_projection()
{
	// Projection
	static float const
		fov = 45.0f,
		near_plane = 1.0f,
		far_plane = 100.0f,
		aspect_ratio = 4.0f / 3.0f;
	return glm::perspective(fov, aspect_ratio, near_plane, far_plane);
}

void MinecraftAssetsPhase::update_camera(GLFWwindow* window, float delta)
{
	// Rotation
	static float const sensitivity = 0.1f;

	static double last_cursor_x = 0, last_cursor_y = 0;
	static bool capture_cursor = false;


	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		double cursor_x, cursor_y;
		glfwGetCursorPos(window, &cursor_x, &cursor_y);

		if (capture_cursor)
		{
			this->camera.offset_rotation(glm::vec2(
				(cursor_x - last_cursor_x) * sensitivity * delta,
				(last_cursor_y - cursor_y) * sensitivity * delta
			));
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			capture_cursor = true;
		}

		last_cursor_x = cursor_x;
		last_cursor_y = cursor_y;
	}
	else if (capture_cursor)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		int width, height;
		glfwGetWindowSize(window, &width, &height);
		glfwSetCursorPos(window, width / 2, height / 2);

		capture_cursor = false;
	}

	// Zoom
	if (glfwGetKey(window, GLFW_KEY_W))
		this->camera.offset_zoom(-0.1f);

	if (glfwGetKey(window, GLFW_KEY_S))
		this->camera.offset_zoom(0.1f);
}

MinecraftAssetsPhase::MinecraftAssetsPhase() :
	camera(glm::vec3(8, 8, 8))
{}


void MinecraftAssetsPhase::update(Stage& stage, float delta)
{
	GLFWwindow* window = stage.window;

	if (this->state == State::VIEW)
	{
		this->update_camera(stage.window, delta);

		// Block slider
		static const double max_delay = 0.500;
		static double delay = max_delay;

		// Issue a block change every certain delay, if the keys are pressed.
		bool left_key = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
		bool right_key = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
		if (left_key || right_key)
		{
			double current_time = glfwGetTime();
			if (current_time - this->last_block_change_time >= delay)
			{
				this->current_block_id += left_key ? -1 : 1;

				auto size = this->assets->block_state_variant_by_id.size();
				if (this->current_block_id < 0) this->current_block_id = 0;
				if (this->current_block_id >= size) this->current_block_id = size - 1;

				delay /= 1.2; // If kept pressed, the scroll becomes faster.
				this->last_block_change_time = glfwGetTime();
			}
		}
		else
		{
			this->last_block_change_time = 0;
			delay = max_delay;
		}
	}
}

void MinecraftAssetsPhase::setup(std::string const& version)
{
	using namespace std::chrono;

	auto current_ms = []() { return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count(); };
	size_t start_ms, end_ms;

	// Loads the assets resources from files.
	start_ms = current_ms();
	this->assets = MinecraftAssets::load(fs::path("resources") / "minecraft_assets", version, [](std::filesystem::path const& file) {});
	end_ms = current_ms();
	std::cout << "Minecraft assets " << version << " loaded in " << (end_ms - start_ms) << "ms." << std::endl;

	// Bakes the atlas texture.
	start_ms = current_ms();
	this->context = MinecraftContext::build(this->assets);
	end_ms = current_ms();
	std::cout << "Minecraft atlas texture baked in " << (end_ms - start_ms) << "ms." << std::endl;

	// Bakes the assets blocks.
	start_ms = current_ms();
	this->baked_assets = MinecraftBakedAssets::bake(this->assets);
	end_ms = current_ms();
	std::cout << "Minecraft assets blocks baked in " << (end_ms - start_ms) << "ms." << std::endl;
}

void MinecraftAssetsPhase::ui_select_minecraft_version(std::string& selected_version, const std::function<void(const std::string&)>& on_load)
{
	if (ImGui::BeginPopupModal("select_minecraft_version", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove))
	{
		// Version
		if (ImGui::BeginCombo("Version", selected_version.c_str()))
		{
			for (const auto& entry : std::filesystem::directory_iterator("resources/minecraft_assets"))
			{
				auto version = entry.path().filename().u8string();
				bool selected = selected_version == version;

				if (ImGui::Selectable(version.c_str(), selected))
					selected_version = version;

				if (selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		// Load
		if (ImGui::Button("Load"))
		{
			on_load(selected_version);
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void MinecraftAssetsPhase::ui_block_info(unsigned int& y)
{
	static const unsigned int padding = 15;
	ImGui::SetNextWindowPos(ImVec2(padding, y + padding));

	if (ImGui::Begin("Block", nullptr, 
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoCollapse | 
		ImGuiWindowFlags_NoNavFocus
	))
	{
		auto& block_by_id = this->assets->block_state_variant_by_id;

		auto block_id = this->current_block_id;
		auto block_name = block_by_id.at(block_id)->first;

		ImGui::Text("ID: %d/%d", block_id + 1, block_by_id.size());
		ImGui::Text("Name: %s", block_name.c_str());

		//ImGui::TextColored(ImVec4(1, 1, 1, 0.7), "Use < > to slide among the loaded blocks.");

		y += padding + ImGui::GetWindowSize().y;
		ImGui::End();
	}
}

void MinecraftAssetsPhase::ui_camera_info(unsigned int& y)
{
	static const unsigned int padding = 15;
	ImGui::SetNextWindowPos(ImVec2(padding, y + padding));

	if (ImGui::Begin("Camera", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse))
	{
		auto position = this->camera.get_position();
		auto rotation = this->camera.get_rotation();

		ImGui::Text("Position: x=%.2f y=%.2f z=%.2f", position.x, position.y, position.z);
		ImGui::Text("Rotation: x=%.2f y=%.2f", rotation.x, rotation.y);
		ImGui::Text("Zoom: %.2f", this->camera.get_zoom());

		ImGui::End();
	}
}

void MinecraftAssetsPhase::ui_menu_bar(unsigned int& y)
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Select version"))
		{
			this->state = State::SELECT_MINECRAFT_VERSION;
			ImGui::EndMenu();
		}

		y += ImGui::GetWindowSize().y;
		ImGui::EndMainMenuBar();
	}
}

void MinecraftAssetsPhase::ui_main()
{
	this->ui_select_minecraft_version(this->version, [&](const std::string& version) {
		this->setup(version);
		this->state = State::VIEW;
	});

	unsigned int y = 0;

	this->ui_menu_bar(y);

	if (this->state == State::SELECT_MINECRAFT_VERSION)
		ImGui::OpenPopup("select_minecraft_version");

	if (this->state == State::VIEW)
	{
		this->ui_block_info(y);
		this->ui_camera_info(y);
	}
}

void MinecraftAssetsPhase::render(Stage& stage)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.1, 0.5, 0.2, 0);

	this->ui_main();

	if (this->baked_assets != nullptr)
		this->world_renderer.render_block(
			get_projection() * this->camera.get_matrix(),
			glm::mat4(1),
			glm::vec4(0, 1, 0, 1),
			this->context,
			this->baked_assets,
			this->current_block_id
		);
}
