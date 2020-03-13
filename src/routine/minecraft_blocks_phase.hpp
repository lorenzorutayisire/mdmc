#pragma once

#include <string>

#include "phase.hpp"

#include "util/camera.hpp"
#include "util/camera_controller.hpp"

class MinecraftBlocksPhase : public Phase
{
public:
	enum class State
	{
		LOADING,
		VIEW
	};

private:
	State state = State::LOADING;

	tdogl::Camera camera;
	CameraController camera_controller;

	std::string selected_version = "1.15.2";

	void load_minecraft_blocks();

	void ui_select_version_modal();

	void ui_menu_bar();

	void ui_main();

public:
	MinecraftBlocksPhase();

	void update(Stage& stage, float delta) override;
	void render(Stage& stage) override;
};
