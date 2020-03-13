#include "minecraft_blocks_phase.hpp"

#include <filesystem>

#include <imgui.h>

MinecraftBlocksPhase::MinecraftBlocksPhase()
{

}

void MinecraftBlocksPhase::update(Stage& stage, float delta)
{
}

void MinecraftBlocksPhase::load_minecraft_blocks()
{


	this->state = State::VIEW;
}

void MinecraftBlocksPhase::ui_select_version_modal()
{
	if (ImGui::BeginPopupModal("#select_version", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove))
	{
		// Version
		if (ImGui::BeginCombo("Version", this->selected_version.c_str()))
		{
			for (const auto& entry : std::filesystem::directory_iterator("resources/minecraft_assets"))
			{
				auto version = entry.path().filename().u8string();

				bool selected = this->selected_version == version;
				if (ImGui::Selectable(version.c_str(), selected))
					this->selected_version = version;
				if (selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}


		// Load
		if (ImGui::Button("Load"))
		{
			this->load_minecraft_blocks();

			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void MinecraftBlocksPhase::ui_menu_bar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Select version"))
		{
			this->state = State::LOADING;
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void MinecraftBlocksPhase::ui_main()
{
	this->ui_menu_bar();

	this->ui_select_version_modal();

	if (this->state == State::LOADING)
		ImGui::OpenPopup("#select_version");
}

void MinecraftBlocksPhase::render(Stage& stage)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0.7, 0.2, 0);

	this->ui_main();
}
