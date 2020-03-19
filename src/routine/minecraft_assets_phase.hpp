#pragma once

#include <string>

#include <GL/glew.h> // #error:  gl.h included before glew.h

#include "phase.hpp"

#include "util/fixed_target_camera.hpp"

#include <tsl/htrie_map.h>

#include "minecraft/minecraft_world.hpp"
#include "minecraft/minecraft_world_renderer.hpp"

#include <imgui.h>

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

	std::string version = "1.15.2";

	std::shared_ptr<MinecraftAssets const> assets;
	std::shared_ptr<MinecraftContext const> context;

	//std::vector<std::pair<std::string, MinecraftBlockStateVariant const*>> block_by_id;
	int current_block_id = 0;
	double last_block_change_time = 0;
	
	FixedTargetCamera camera;
	void update_camera(GLFWwindow* window, float delta);

	std::shared_ptr<MinecraftBakedAssets const> baked_assets;
	MinecraftWorldRenderer world_renderer;

	void setup(std::string const& version);

	void ui_menu_bar(unsigned int& y);

	void ui_select_minecraft_version(std::string& current_version, const std::function<void(const std::string&)>& on_load);
	
	void ui_block_info(unsigned int& y);
	void ui_camera_info(unsigned int& y);

	void ui_main();

public:
	MinecraftAssetsPhase();

	void update(Stage& stage, float delta) override;
	void render(Stage& stage) override;
};
