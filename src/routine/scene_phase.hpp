#pragma once

#include <memory>
#include <filesystem>

#include "util/gl.hpp"

#include "phase.hpp"
#include "field.hpp"

#include "util/camera.hpp"
#include "util/camera_controller.hpp"

#include "scene/scene_renderer.hpp"

namespace mdmc
{
	class scene_phase : public Phase
	{
	public:
		enum class State
		{
			LOADING,
			VIEW
		};

	private:
		State state = scene_phase::State::LOADING;

		char scene_filename[512];

		std::shared_ptr<Scene> scene;
		glm::mat4 transform;

		tdogl::Camera camera;
		CameraController camera_controller;

		SceneRenderer scene_renderer;

		Material::Type selected_view_type = Material::Type::DIFFUSE;

		void download_minecraft_assets(char const* version);
		void load_scene(const std::filesystem::path& path);

		void ui_load_scene_modal();
		bool ui_next_button(Stage& stage);

		void add_load_menu();
		void add_view_type_menu();

	public:
		void update(Stage& stage, float delta) override;
		void render(Stage& stage) override;
	};
}

