#pragma once

#include <string>

#include <GL/glew.h> // #error:  gl.h included before glew.h

#include "phase.hpp"

#include "util/camera.hpp"
#include "util/camera_controller.hpp"

#include "minecraft/minecraft_world.hpp"
#include "minecraft/minecraft_world_renderer.hpp"

class MinecraftAssetsPhase : public Phase
{
public:
	enum class State
	{
		SELECT_MINECRAFT_VERSION,
		VIEW
	};

private:
	State state = State::SELECT_MINECRAFT_VERSION;

	tdogl::Camera camera;
	CameraController camera_controller;

	std::string version = "1.15.2";

	std::shared_ptr<MinecraftAssets const> assets;
	std::shared_ptr<MinecraftContext const> context;

	std::shared_ptr<MinecraftWorld> world;
	MinecraftWorldRenderer world_renderer;

	void setup(std::string const& version);

	void ui_select_minecraft_version(std::string& current_version, const std::function<void(const std::string&)>& on_load);
	void ui_loading_logs();

	void ui_menu_bar();
	void ui_main();

public:
	MinecraftAssetsPhase();

	void update(Stage& stage, float delta) override;
	void render(Stage& stage) override;
};
