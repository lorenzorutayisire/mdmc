
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

#include "Chunk.hpp"

#include "Voxelizer/aiSceneWrapper.hpp"
#include "Voxelizer/Voxelizer.hpp"

#define MAX_SIZE_PER_CHUNK 1000000

// RenderDoc

#define DEBUG

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

void test_voxelize(Voxelizer& voxelizer, std::shared_ptr<const Voxelizer::Field> model)
{
	int prev_frag = -1;

	for (int height = 1; height <= 256; height++)
	{
		std::cout << "***" << std::endl;
		for (int resolution = 1; resolution <= 9; resolution++)
		{
			float ratio = height / model->size().y;
			glm::uvec3 size = glm::ceil(ratio * model->size());

			unsigned int largest_side = glm::max(size.x, glm::max(size.y, size.z));
			GLuint viewport_side = largest_side * resolution;

			auto volume = voxelizer.voxelize(model, height, resolution);
			std::cout <<
				" - height: " << height <<
				" - resolution: " << resolution <<
				" - fragments: " << volume->count <<
				" - viewport: " << viewport_side << "x" << viewport_side;

			if (prev_frag > volume->count && resolution > 1)
				std::cout << " <----";

			prev_frag = volume->count;
			std::cout << std::endl;
		}
	}
}

void test_sort(Voxelizer& voxelizer, std::shared_ptr<const Volume> volume)
{
	voxelizer.sort(volume);
}

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
	int resolution = atoi(argv[3]);
	if (resolution <= 0)
	{
		std::cerr << "resolution can't be neither negative, null or higher than 3." << std::endl;
		return 4;
	}

	uint16_t height = (uint16_t)raw_height;

	// <minecraft_version>
	filesystem::path mc_path(filesystem::path("tmp") / "mc_assets" / std::string(argv[4]));

	std::cout << "Input:"								<< std::endl;
	std::cout << "height: " << height					<< std::endl;
	std::cout << "resolution: " << resolution			<< std::endl;
	std::cout << "model_path: \"" << model_path << "\"" << std::endl;
	std::cout << "minecraft_version: " << argv[4]		<< std::endl;

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
	//test_voxelize(voxelizer, model);

	auto volume = voxelizer.voxelize(model, height, resolution);
	voxelizer.sort(volume);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}

	return 0;
}
