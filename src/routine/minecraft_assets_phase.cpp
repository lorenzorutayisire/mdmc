#include "minecraft_assets_phase.hpp"

#include <filesystem>
#include <iostream>
#include <thread>
#include <chrono>
#include <iostream>

#include "util/render_doc.hpp"

#include <imgui.h>
#include <zip.h>

#include <glm/gtx/transform.hpp>

#ifdef WIN32
	#include <shlobj_core.h>
#endif

using namespace std::chrono;
using ms = std::chrono::milliseconds;

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

MinecraftAssetsPhase::MinecraftAssetsPhase() :
	m_camera(glm::vec3(8, 8, 8))
{}

std::shared_ptr<Octree> MinecraftAssetsPhase::voxelize(unsigned int resolution)
{
	uint32_t volume_side = glm::exp2(resolution);

	auto& block = m_baked_mc_blocks->m_blocks.at(m_current_block_id);

	std::cout << "Voxelization process started..." << std::endl;
	std::cout << "Vertices pool: ["  << block.m_start_at << "," << block.m_count << "]" << std::endl;
	std::cout << "Resolution: "		 << resolution     << std::endl;
	std::cout << "Side: "			 << volume_side    << std::endl;

	ms start_at = duration_cast<ms>(system_clock::now().time_since_epoch());

	this->octree = std::make_shared<Octree>(resolution);
	std::shared_ptr<VoxelList> voxel_list = m_mc_blocks_voxelizer.voxelize(block, volume_side);
	this->octree_builder.build(voxel_list, octree);

	ms end_at = duration_cast<ms>(system_clock::now().time_since_epoch());

	std::cout << "Generated voxels: "	   << voxel_list->size << std::endl;
	std::cout << "Generated octree size: " << octree->capacity / 1024 << " KB" << std::endl;

	std::cout << "Done in " << (end_at - start_at).count() << "ms" << std::endl;

	return octree;
}

void MinecraftAssetsPhase::toggle_view_block_octree_mode()
{
	this->view_block_octree = !this->view_block_octree;
	if (this->view_block_octree && this->octree == nullptr)
	{
		this->octree = this->voxelize(5);
	}
}

void MinecraftAssetsPhase::enable(Stage& stage)
{
	this->glfw_cb_handler = std::make_unique<GlfwCallbackHandler>(stage.window);

	this->glfw_cb_handler->sub_key_callback(GLFW_KEY_V, GLFW_PRESS, [&]() {
		this->toggle_view_block_octree_mode();
	});
}

void MinecraftAssetsPhase::disable(Stage& stage)
{
	this->glfw_cb_handler = nullptr;
}

/*
	Tests and updates the camera movements.
*/
void MinecraftAssetsPhase::test_camera_input(GLFWwindow* window, float delta)
{
	if (ImGui::IsAnyWindowHovered())
		return;

	static float const sensitivity = 0.1f;

	static double last_cursor_x = 0, last_cursor_y = 0;
	static bool capture_cursor = false;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		double cursor_x, cursor_y;
		glfwGetCursorPos(window, &cursor_x, &cursor_y);

		if (capture_cursor)
		{
			this->m_camera.offset_rotation(glm::vec2(
				(cursor_x - last_cursor_x) * sensitivity * delta,
				(cursor_y - last_cursor_y) * sensitivity * delta
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
		this->m_camera.offset_zoom(-0.1f);

	if (glfwGetKey(window, GLFW_KEY_S))
		this->m_camera.offset_zoom(0.1f);
}

/*
	Tests and manages the sliding among Minecraft blocks.
*/
void MinecraftAssetsPhase::test_block_sliding_input(GLFWwindow* window, float delta)
{
	static const double max_delay = 0.500;
	static double delay = max_delay;

	bool left_key = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
	bool right_key = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
	if (left_key || right_key)
	{
		double current_time = glfwGetTime();
		if (current_time - this->last_block_change_time >= delay)
		{
			move_block_id(right_key);

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

void MinecraftAssetsPhase::set_current_block_id(int block_id)
{
	m_current_block_id = glm::clamp<int>(block_id, 0, (int) m_mc_assets->m_block_state_variant_by_id.size());
	this->view_block_octree = false;
	this->octree = nullptr;

	m_mc_baked_world->set_block(glm::ivec3(0, 0, 0), m_current_block_id);
	m_mc_baked_world->rebuild();
}

void MinecraftAssetsPhase::move_block_id(bool forward)
{
	set_current_block_id(m_current_block_id + (forward ? 1 : -1));
}

void MinecraftAssetsPhase::update(Stage& stage, float delta)
{
	GLFWwindow* window = stage.window;

	if (this->state == State::VIEW)
	{
		this->test_camera_input(stage.window, delta);
		this->test_block_sliding_input(stage.window, delta);
	}
}

std::filesystem::path get_mc_data_folder()
{
#ifdef WIN32
	TCHAR appdata_path[MAX_PATH];
	SHGetFolderPath(nullptr, CSIDL_APPDATA, nullptr, 0, appdata_path);

	return std::filesystem::path(appdata_path) / ".minecraft";
#else
	#error "Only WIN32 is supported!"
#endif
}

std::vector<std::string> get_available_mc_versions()
{
	std::vector<std::string> mc_versions;
	for (auto const& mc_version : std::filesystem::directory_iterator(get_mc_data_folder() / "versions")) {
		if (mc_version.is_directory()) {
			mc_versions.push_back(mc_version.path().filename().u8string());
		}
	}
	return mc_versions;
}

void MinecraftAssetsPhase::setup(std::string const& mc_version)
{
	auto jar = get_mc_data_folder() / "versions" / mc_version / (mc_version + ".jar");
	int err;
	zip* mc_jar = zip_open(jar.u8string().c_str(), 0, &err);
	if (err == NULL) {
		throw std::runtime_error("Can't unzip Minecraft .jar file");
	}

	// Loading
	std::cout << "Loading Minecraft " << mc_version << " assets..." << std::endl;

	m_mc_assets = std::make_shared<mdmc::mc_assets>();
	m_mc_assets->from_jar(mc_jar, mc_version);

	m_mc_baked_world = std::make_unique<mdmc::mc_baked_world>(m_mc_assets);
	set_current_block_id(0);

	// Baking
	std::cout << "Baking assets for voxelization & debug rendering..." << std::endl;

	m_baked_mc_blocks = std::make_shared<mdmc::baked_mc_blocks>();
	m_baked_mc_blocks->from_mc_assets(*m_mc_assets);

	std::cout << "Done" << std::endl;
}

using on_mc_version_select = std::function<void(std::string&)>;

void ui_select_mc_version(std::string& selected_version, on_mc_version_select const& callback)
{
	if (ImGui::BeginPopupModal(
		"select_minecraft_version",
		nullptr,
		ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove
		))
	{
		// Version selector
		if (ImGui::BeginCombo("Select a version", selected_version.c_str()))
		{
			for (auto const& mc_version : get_available_mc_versions())
			{
				bool selected = selected_version == mc_version;
				if (ImGui::Selectable(mc_version.c_str(), selected)) {
					selected_version = mc_version;
				}
				if (selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		// Load
		if (ImGui::Button("Load"))
		{
			callback(selected_version);
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

/*
	Choose the resolution to use and run the voxelization process.
*/
void MinecraftAssetsPhase::ui_voxelization_options()
{
	if (ImGui::BeginPopupModal("voxelization_options",
		NULL,
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove))
	{
		ImGui::SliderInt("Resolution", (int*) &this->resolution, 1, 9);

		if (ImGui::Button("Change"))
		{
			this->view_block_octree = true;
			this->state = State::VIEW;
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
		auto& block_state_variant = m_mc_assets->m_block_state_variant_by_id.at(m_current_block_id);

		ImGui::Text("ID: %d/%llu", m_current_block_id + 1, m_mc_assets->m_block_state_variant_by_id.size());
		ImGui::Text("Name: %s", block_state_variant.first.c_str());

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
		auto position = this->m_camera.get_position();
		auto rotation = this->m_camera.get_rotation();

		ImGui::Text("Position: x=%.2f y=%.2f z=%.2f", position.x, position.y, position.z);
		ImGui::Text("Rotation: x=%.2f y=%.2f", rotation.x, rotation.y);
		ImGui::Text("Zoom: %.2f", this->m_camera.get_zoom());

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

		if (ImGui::BeginMenu("Voxelization options"))
		{
			this->state = State::VOXELIZATION_OPTIONS;
			ImGui::EndMenu();
		}

		y += ImGui::GetWindowSize().y;
		ImGui::EndMainMenuBar();
	}
}

void MinecraftAssetsPhase::ui_main()
{
	ui_select_mc_version(m_version, [&](std::string const& version) {
		setup(version);
		state = State::VIEW;
	});
	this->ui_voxelization_options();

	unsigned int y = 0;

	this->ui_menu_bar(y);

	switch (this->state)
	{
	case State::SELECT_MINECRAFT_VERSION:
		ImGui::OpenPopup("select_minecraft_version");
		break;

	case State::VOXELIZATION_OPTIONS:
		ImGui::OpenPopup("voxelization_options");
		break;

	case State::VIEW:
		ui_block_info(y);
		ui_camera_info(y);
		break;
	}
}

void MinecraftAssetsPhase::render(Stage& stage)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0xA5 / float(0xFF), 0xD6 / float(0xFF), 0xA7 / float(0xFF), 0);

	this->ui_main();

	if (!this->view_block_octree && this->m_baked_mc_blocks != nullptr)
	{
		//RenderDoc().capture([this] {
		glm::mat4 camera = get_projection() * m_camera.get_matrix();
		m_mc_renderer.render(*m_mc_baked_world, camera, glm::mat4(1));
		//});
	}
	else if (this->view_block_octree && this->octree != nullptr)
	{
		int width, height;
		glfwGetWindowSize(stage.window, &width, &height);
		
		this->octree_tracer.render(
			glm::uvec2(width, height),

			glm::vec3(0), glm::vec3(16),

			get_projection(),
			this->m_camera.get_matrix(),
			this->m_camera.get_position(),
			this->octree
		);
	}
}
