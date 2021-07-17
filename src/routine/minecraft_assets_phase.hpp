#pragma once

#include <string>

#include <GL/glew.h> // #error:  gl.h included before glew.h

#include "phase.hpp"

#include "util/fixed_target_camera.hpp"

#include <tsl/htrie_map.h>

#include "mc/mc_assets.hpp"
#include "mc/mc_blocks_voxelizer.hpp"
#include "mc/mc_renderer.hpp"

#include "octree/octree.hpp"
#include "octree/octree_builder.hpp"
#include "octree/octree_tracer.hpp"

#include "util/glfw_callback_handler.hpp"

#include <imgui.h>

class MinecraftAssetsPhase : public Phase
{
public:
	enum class State
	{
		SELECT_MINECRAFT_VERSION,
		VOXELIZATION_OPTIONS,
		VIEW
	};

private:
	State state = State::SELECT_MINECRAFT_VERSION;

	OctreeBuilder octree_builder;
	OctreeRenderer octree_tracer;

	unsigned int resolution = 1;

	int view_block_octree = false;
	std::shared_ptr<Octree> octree;

	std::string m_version = "1.15.2";

	mdmc::mc_blocks_voxelizer m_mc_blocks_voxelizer;
	mdmc::mc_renderer m_mc_renderer;

	std::unique_ptr<mdmc::mc_baked_world> m_mc_baked_world;
	std::shared_ptr<mdmc::mc_assets> m_mc_assets;
	std::shared_ptr<mdmc::baked_mc_blocks> m_baked_mc_blocks;

	//std::vector<std::pair<std::string, MinecraftBlockStateVariant const*>> block_by_id;
	int m_current_block_id = 0;

	void set_current_block_id(int block_id);
	void move_block_id(bool forward);

	double last_block_change_time = 0;

	FixedTargetCamera m_camera;
	void test_camera_input(GLFWwindow* window, float delta);
	void test_block_sliding_input(GLFWwindow* window, float delta);

	// todo MinecraftRenderer minecraft_renderer;

	std::unique_ptr<GlfwCallbackHandler> glfw_cb_handler;

	void setup(std::string const& version);

	void ui_menu_bar(unsigned int& y);

	std::shared_ptr<Octree> voxelize(unsigned int resolution);

	void ui_voxelization_options();

	void ui_block_info(unsigned int& y);
	void ui_camera_info(unsigned int& y);

	void ui_main();

public:
	MinecraftAssetsPhase();

	void toggle_view_block_octree_mode();

	void enable(Stage& stage) override;
	void disable(Stage& stage) override;

	void update(Stage& stage, float delta) override;
	void render(Stage& stage) override;
};
