
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
#include "Minecraft/Mapper.hpp"

#include "Chunk.hpp"

#include "Voxelizer/aiSceneWrapper.hpp"

#define MAX_SIZE_PER_CHUNK 1000000

// RenderDoc

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

	if (argc != (1 + 4)) // The first is the name of the exec.
	{
		std::cerr << "Wrong arguments: <model_path> <height> <resolution> <minecraft_version>" << std::endl;
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

	if (raw_height <= 0 || raw_height > 256)
	{
		std::cerr << "height can't be neither negative, null or higher than 256." << std::endl;
		return 3;
	}

	// <resolution>
	const int resolution = atoi(argv[3]);
	if (resolution <= 0 || resolution > 3)
	{
		std::cerr << "resolution can't be neither negative, null or higher than 3." << std::endl;
		return 4;
	}

	const uint16_t height = (uint16_t)raw_height;

	// <minecraft_version>
	filesystem::path mc_path(filesystem::path("tmp") / "mc_assets" / std::string(argv[4]));
	if (!std::filesystem::exists(mc_path))
	{
		std::cerr << mc_path << " not found." << std::endl;
		return 2;
	}

	if (glfwInit() != GLFW_TRUE)
	{
		std::cerr << "GLFW failed to initialize." << std::endl;
		return 7;
	}

	GLFWwindow* window = glfwCreateWindow(500, 500, "mdmc", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwHideWindow(window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "GLEW failed to initialize." << std::endl;
		return 6;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_3D);

	auto model = load_model(model_path);

	Voxelizer voxelizer;

	RDOC_API_CAPTURE(
		std::shared_ptr<const Volume> volume = voxelizer.voxelize(model, height, resolution);
	);

	/*
	unsigned int
		model_volume_side = model->largest_side() * height / model->size().y, // resolution!
		model_volume_size = pow(model_volume_side, 3);


	float max_chunk_size_side = cbrt(MAX_SIZE_PER_CHUNK / (float)4) / (float)resolution;

	unsigned int
		chunk_volume_side = ceil(min((float) model_volume_side, max_chunk_size_side)),
		chunk_volume_size = pow(chunk_volume_side, 3);

	float chunk_side = chunk_volume_side * model->size().y / height;

	Minecraft::Mapper mapper;

	auto assets = load_assets(filesystem::path("tmp") / "mc_assets", "1.14");
	auto assets_volume = voxelizer.voxelize(assets, assets->size().y / 16, resolution);
	auto assets_palette = Minecraft::BlocksPalette(1, assets->get_blocks().size(), assets_volume);

	unsigned int i = 0;

	for (float x = model->min.x; x <= model->max.x; x += chunk_side)
	{
		for (float y = model->min.y; y <= model->max.y; y += chunk_side)
		{
			for (float z = model->min.z; z <= model->max.z; z += chunk_side)
			{
				auto chunk = std::make_shared<mdmc::Chunk>(model, glm::vec3(x, y, z), chunk_side);


				auto mapper_result = mapper.map(chunk_volume, assets_palette);

			}
		}
	}
	*/

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}

	return 0;
}
