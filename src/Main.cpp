
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>

#include "PhaseManager.hpp"

#include "Minecraft/Assets.hpp"
#include "Minecraft/Mapper.hpp"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "Voxelizer/aiSceneWrapper.hpp"
#include "Routine/ViewFieldPhase.hpp"

// RenderDoc
#include <windows.h>
#include "renderdoc_app.h"

#define resolution 1

#define DEBUG

#define RDOC_API_CAPTURE(code) \
	if (rdoc_api) rdoc_api->StartFrameCapture(NULL, NULL); \
	code \
	if (rdoc_api) rdoc_api->EndFrameCapture(NULL, NULL)

using namespace std;

std::shared_ptr<const mdmc::aiSceneWrapper> load_model(const filesystem::path& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.u8string().c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);

	if (scene == nullptr)
		return nullptr;

	return std::make_shared<mdmc::aiSceneWrapper>(scene, path.parent_path());
}

std::shared_ptr<const mdmc::Minecraft::Assets> load_assets(const filesystem::path& path, const string& version)
{
	auto assets = std::make_shared<mdmc::Minecraft::Assets>(path, version);
	assets->load();
	return assets;
}


int main(int argc, char** argv)
{
	RENDERDOC_API_1_1_2* rdoc_api = NULL;

	// At init, on windows
	if (HMODULE mod = GetModuleHandleA("renderdoc.dll"))
	{
		pRENDERDOC_GetAPI RENDERDOC_GetAPI =
			(pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
		int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void**)&rdoc_api);
		assert(ret == 1);
	}

	/* ARGS */
	// Gets from the command-line the required arguments.

	if (argc != (1 + 3)) // The first is the name of the exec.
	{
		std::cerr << "Wrong arguments: <model_path> <height> <minecraft_version>" << std::endl;
		return 1;
	}

	// <model_path>
	filesystem::path model_path = argv[1];
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
	filesystem::path mc_path(filesystem::path("tmp") / "mc_assets" / std::string(argv[3]));
	if (!std::filesystem::exists(mc_path))
	{
		std::cerr << mc_path << " not found." << std::endl;
		return 2;
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

	glfwShowWindow(window); // Now the window can be shown.

	// Loading

	auto model = load_model(model_path);
	auto assets = load_assets(filesystem::path("tmp") / "mc_assets", "1.14");

	// Voxelization

	std::cout << "Voxelization process (height=" << height << ", resolution=" << resolution << ")..." << std::endl;

	Voxelizer voxelizer;

	RDOC_API_CAPTURE(
		std::cout << "Voxelizing Model..." << std::endl;
		auto model_volume = voxelizer.voxelize(model, height, resolution);
		std::cout << "Done." << std::endl;
	);

	RDOC_API_CAPTURE(
		std::cout << "Voxelizing Minecraft assets..." << std::endl;
		auto assets_volume = voxelizer.voxelize(assets, assets->size().y / 16, resolution);
		std::cout << "Done." << std::endl;
	);

	// Minecraft Mapping

	Minecraft::Mapper mapper;

	std::cout << "Minecraft mapping (blocks=" << assets->get_blocks().size() << ")..." << std::endl;

	RDOC_API_CAPTURE(
		Minecraft::BlocksPalette blocks_palette(1, assets->get_blocks().size(), assets_volume);
		auto mapping_result = mapper.map(model_volume, blocks_palette);
	);

	std::cout << "Done." << std::endl;


	bool show_demo_window = true;

	int w = 0;

	int wi, he;
	glfwGetWindowSize(window, &wi, &he);
	glViewport(0, 0, wi, he);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(1, 1, 0, 0);
		
		// Update

		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
			w = 0;

		if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
			w = 1;

		if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
			w = 2;

		assets->test_render(w);

		// Closes the window when ESCAPE key is pressed.
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}

		glfwSwapBuffers(window);


		//phase_manager.on_update(1.0f);

		/* Rendering */
		//phase_manager.on_render();

		//phase_manager.on_render_ui();

	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
