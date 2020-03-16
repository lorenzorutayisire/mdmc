#include "scene_phase.hpp"

#include <iostream>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>
#include <imgui_internal.h>

#include <tinyfiledialogs.h>

#include "scene/ai_scene_loader.hpp"
#include "minecraft_assets_phase.hpp"

void ScenePhase::update(Stage& stage, float delta)
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

void ScenePhase::load_scene(const std::filesystem::path& path)
{
	this->scene = aiSceneLoader::load(path);

	this->state = State::VIEW;
}

void ScenePhase::ui_load_scene_modal()
{
	if (ImGui::BeginPopupModal("load_scene", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove))
	{
		ImGui::InputText("", this->scene_filename, 512);

		ImGui::SameLine();

		if (ImGui::Button("...", ImVec2(25, 0)))
		{
			const char* const filters[]{ "*.gltf", "*.obj", "*.fbx", "*.blend" };
			const int filters_count = 4;

			const char* chosen_filename = tinyfd_openFileDialog("Load scene", "", filters_count, filters, nullptr, false);
			if (chosen_filename) strcpy(this->scene_filename, chosen_filename);

		}

		bool scene_exists = std::filesystem::exists(this->scene_filename);

		if (!scene_exists)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}

		if (ImGui::Button("Load"))
		{
			this->load_scene(this->scene_filename);

			ImGui::CloseCurrentPopup();
		}

		if (!scene_exists)
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}

		ImGui::EndPopup();
	}
}

bool ScenePhase::ui_next_button(Stage& stage)
{
	auto display_size = ImGui::GetIO().DisplaySize;
	auto window_size = ImVec2(256, 64);

	ImGui::Begin("next_button", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);

	ImGui::SetWindowSize(window_size);
	ImGui::SetWindowPos(ImVec2(display_size.x - window_size.x - 10, display_size.y - window_size.y - 10));

	auto button_size = ImVec2(window_size.x, window_size.y * 0.75f);
	if (ImGui::Button("Next", button_size))
	{
		auto next_phase = std::make_shared<MinecraftAssetsPhase>();
		stage.set_phase(next_phase);
	}

	ImGui::End();
	return false;
}

void ScenePhase::add_load_menu()
{
	if (ImGui::BeginMenu("Load"))
	{
		this->state = ScenePhase::State::LOADING;
		ImGui::EndMenu();
	}
}

void ScenePhase::add_view_type_menu()
{
	const char* view_types[]{ "None", "Diffuse", "Ambient", "Specular", "Emissive" };

	if (ImGui::BeginMenu((std::string("Lighting type: ") + view_types[this->selected_view_type]).c_str()))
	{
		for (size_t view_type = 0; view_type < Material::Type::size; view_type++)
		{
			if (ImGui::MenuItem(view_types[view_type]))
				this->selected_view_type = static_cast<Material::Type>(view_type);
		}

		ImGui::EndMenu();
	}
}

void ScenePhase::render(Stage& stage)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (ImGui::BeginMainMenuBar())
	{
		this->add_load_menu();
		this->add_view_type_menu();

		ImGui::EndMainMenuBar();
	}

	this->ui_next_button(stage);

	this->ui_load_scene_modal();

	switch (this->state)
	{
	case ScenePhase::State::LOADING:
		ImGui::OpenPopup("load_scene");

		glClearColor(0.1, 0.1, 0.6, 0);
		break;

	case ScenePhase::State::VIEW:

		glClearColor(0.6, 0.1, 0.1, 0);

		scene_renderer.render(this->camera.matrix(), glm::mat4(1), this->scene, this->selected_view_type);
		break;
	}
}
