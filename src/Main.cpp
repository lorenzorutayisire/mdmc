
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>

#include <imgui.h>
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "PhaseManager.hpp"

#include "Minecraft/TextureAsset.hpp"
#include "Minecraft/Assets.hpp"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "Scene/aiSceneRenderer.hpp"
#include "Scene/ViewScenePhase.hpp"

using namespace mdmc;

void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n", (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
}


int main(int argc, char** argv)
{
	/* ARGS */
	// Gets from the command-line the required arguments.

	if (argc != (1 + 3)) // The first is the name of the exec.
	{
		std::cerr << "Wrong arguments: <model_path> <height> <minecraft_version>" << std::endl;
		return 1;
	}

	// <model_path>
	const std::filesystem::path model_path = argv[1];
	if (!std::filesystem::exists(model_path))
	{
		std::cerr << "Can't open file at: " << model_path << std::endl;
		return 2;
	}

	// <height>
	const int raw_height = atoi(argv[2]);

#ifndef UNLIMITED_HEIGHT
	if (raw_height > 256)
	{
		std::cerr << "A Minecraft schematic can't be taller than 256 blocks." << std::endl;
		return 3;
	}
#endif
	if (raw_height <= 0)
	{
		std::cerr << "A Minecraft schematic can't be negative or 0." << std::endl;
		return 3;
	}

	const uint16_t height = (uint16_t)raw_height;

	// <minecraft_version>
	const std::string minecraft_version = argv[3];
	std::ifstream minecraft_asset_file("resources/minecraft_assets/" + minecraft_version + ".bin", std::ios::binary);
	if (!minecraft_asset_file.good())
	{
		std::cerr << "Unsupported Minecraft version: " + minecraft_version + ".";
		return 4;
	}

	/* INIT */
	// Initializes GLFW & GLEW stuff.

	if (glfwInit() != GLFW_TRUE)
	{
		std::cerr << "GLFW failed to initialize." << std::endl;
		return 5;
	}

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // Must be placed before window creation.

	GLFWwindow* window = glfwCreateWindow(712, 712, "MDMC", NULL, NULL);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // OpenGL 4.2 required to run Image load/store extension.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE); // To make MacOS happy, should not be needed.
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL. 
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "GLEW failed to initialize." << std::endl;
		return 6;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_3D);

	glEnable(GL_DEBUG_OUTPUT); // Enables OpenGL warning & error logging.
	glDebugMessageCallback(MessageCallback, 0);

	glfwShowWindow(window); // Now the window can be shown.


	// ImGui context setup.
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Platform/Renderer bindings.
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");


	/* Minecraft loading */
	//Minecraft::Assets assets("tmp/mc_assets", "1.14.4");
	//assets.retrieve();
	//assets.load();

	/* Scene loading */
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(model_path.u8string().c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
	if (scene == nullptr)
	{
		std::cerr << importer.GetErrorString() << std::endl;
		return 1;
	}

	aiSceneRenderer scene_renderer(scene, model_path.parent_path());

	PhaseManager phase_manager(window);
	phase_manager.set_phase(new ViewScenePhase(scene_renderer));

	bool show_demo_window = true;

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.5, 0.5, 0.5, 0);
		
		// Update

		glfwPollEvents();

		// Closes the window when ESCAPE key is pressed.
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}

		phase_manager.on_update(1.0f);

		/* Rendering */
		phase_manager.on_render();

		/* UI Rendering (ImGui) */
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		phase_manager.on_render_ui();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	// ImGui cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
