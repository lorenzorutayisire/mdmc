
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>
#include <memory>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <windows.h>
#include "renderdoc_app.h"

#include "Minecraft/Assets.hpp"

#include "routine/phase.hpp"

#include "scene/scene.hpp"
#include "scene/ai_scene_loader.hpp"

#include "routine/scene_phase.hpp"

// imgui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#define MAX_SIZE_PER_CHUNK 1000000

// RenderDoc

#define DEBUG


#define WINDOW_WIDTH  1024
#define WINDOW_HEIGHT 720

using namespace std;


int main(int argc, char** argv)
{
	/* ARGS */
	// Gets from the command-line the required arguments.

	if (argc != (1 + 4)) // The first is the name of the exec.
	{
		std::cerr << "Wrong arguments: <model_path> <height> <resolution> <minecraft_version>" << std::endl;
		return 1;
	}

	// <model_path>
	filesystem::path scene_path = argv[1];
	if (!std::filesystem::exists(scene_path))
	{
		std::cerr << "Can't open file at: " << scene_path << std::endl;
		return 2;
	}

	// <height>
	const int raw_height = atoi(argv[2]);

	if (raw_height <= 0 || raw_height > 256)
	{
		std::cerr << "height can't be neither negative, null or higher than 256." << std::endl;
		return 3;
	}

	// <resolution>
	int resolution = atoi(argv[3]);
	if (resolution <= 0)
	{
		std::cerr << "resolution can't be neither negative, null or higher than 3." << std::endl;
		return 4;
	}

	uint16_t height = (uint16_t)raw_height;

	// <minecraft_version>
	filesystem::path mc_path(filesystem::path("tmp") / "mc_assets" / std::string(argv[4]));

	std::cout << "Height: " << height << std::endl;
	std::cout << "Resolution: " << resolution << std::endl;
	std::cout << "Scene path: " << scene_path << std::endl;
	std::cout << "Minecraft version: " << argv[4] << std::endl;

	if (glfwInit() != GLFW_TRUE)
	{
		std::cerr << "GLFW failed to initialize." << std::endl;
		return 7;
	}

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "mdmc", NULL, NULL);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "GLEW failed to initialize." << std::endl;
		return 6;
	}

	glfwShowWindow(window);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.Fonts->AddFontFromFileTTF("resources/fonts/Baloo2-Regular.ttf", 24.0f);

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	Stage stage(window);

	auto phase = std::make_shared<ScenePhase>();
	stage.set_phase(phase);

	double prior_time = 0;

	while (!glfwWindowShouldClose(window))
	{
		// Update
		glfwPollEvents();

		double time = glfwGetTime();

		float delta = prior_time == 0 ? 0 : time - prior_time;
		prior_time = time;

		stage.update(delta);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		glViewport(0, 0, width, height);

		stage.render();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
