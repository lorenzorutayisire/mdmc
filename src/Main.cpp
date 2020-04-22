
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

#include "routine/phase.hpp"

#include "scene/scene.hpp"
#include "scene/ai_scene_loader.hpp"

#include "routine/scene_phase.hpp"
#include "routine/minecraft_assets_phase.hpp"

// imgui
#include <imgui.h>
#include <imgui_freetype.h>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#define MAX_SIZE_PER_CHUNK 1000000

#define WINDOW_WIDTH  1024
#define WINDOW_HEIGHT 720

using namespace std;

int main(int argc, char** argv)
{
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

	ImGuiIO& io = ImGui::GetIO(); (void) io;
	ImFontConfig font_cfg;
    font_cfg.RasterizerMultiply = 1.1f;
	font_cfg.RasterizerFlags = ImGuiFreeType::ForceAutoHint;
	io.Fonts->AddFontFromFileTTF("resources/fonts/Roboto-Regular.ttf", 16.0f, &font_cfg);
    ImGuiFreeType::BuildFontAtlas(io.Fonts);

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	Stage stage(window);

	auto phase = std::make_shared<MinecraftAssetsPhase>();
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

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.75, 0.75, 0, 0);

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
