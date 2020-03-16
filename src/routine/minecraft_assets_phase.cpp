#include "minecraft_assets_phase.hpp"

#include <filesystem>
#include <iostream>
#include <thread>
#include <chrono>
#include <iostream>

#include <imgui.h>

#include "minecraft/assets/minecraft_assets.hpp"

namespace fs = std::filesystem;

MinecraftAssetsPhase::MinecraftAssetsPhase()
{}

void MinecraftAssetsPhase::update(Stage& stage, float delta)
{
	GLFWwindow* window = stage.window;

	if (this->state == State::VIEW)
	{
		bool speed_boost = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

		bool is_camera_moving = false;
		is_camera_moving |= this->camera_controller.process_camera_movement(window, this->camera, 10 * (speed_boost ? 10 : 1) * delta);
		is_camera_moving |= this->camera_controller.process_camera_rotation(window, this->camera, 10 * delta);
	}
}

void MinecraftAssetsPhase::setup(std::string const& version)
{
	using namespace std::chrono;

	auto current_ms = []() { return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count(); };
	size_t start_ms, end_ms;

	start_ms = current_ms();
	this->assets = MinecraftAssets::load(fs::path("resources") / "minecraft_assets", version, [](std::filesystem::path const& file) {});
	end_ms = current_ms();
	std::cout << "Minecraft assets " << version << " loaded in " << (end_ms - start_ms) << "ms." << std::endl;

	start_ms = current_ms();
	this->context = MinecraftContext::build(this->assets);
	end_ms = current_ms();
	std::cout << "Minecraft assets built in " << (end_ms - start_ms) << "ms." << std::endl;

	this->world = std::make_shared<MinecraftWorld>(this->context);

	start_ms = current_ms();
	glm::vec3 block_position(0);
	for (auto const& block_state : this->assets->block_state_by_name)
	{
		for (auto const& block_state_variant : block_state.second.variant_by_name)
		{
			this->world->set_block(block_position, block_state_variant.second, false);
			block_position.z += 16; // The blocks are placed along the Z axis.
		}
	}
	this->world->build();
	end_ms = current_ms();
	std::cout << "Minecraft world built in " << (end_ms - start_ms) << "ms." << std::endl;
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

void MinecraftAssetsPhase::ui_menu_bar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Select version"))
		{
			this->state = State::SELECT_MINECRAFT_VERSION;
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void MinecraftAssetsPhase::ui_main()
{
	this->ui_select_minecraft_version(this->version, [&](const std::string& version) {
		this->setup(version);
		this->state = State::VIEW;
	});
	this->ui_menu_bar();

	if (this->state == State::SELECT_MINECRAFT_VERSION)
		ImGui::OpenPopup("select_minecraft_version");
}

void MinecraftAssetsPhase::render(Stage& stage)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.1, 0.5, 0.2, 0);

	this->ui_main();

	if (this->world != nullptr)
		this->world_renderer.render(this->camera.matrix(), glm::mat4(1), world, glm::vec4(0, 1, 0, 1));
}
