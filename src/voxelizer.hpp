#pragma once

#include <vector>
#include <memory>
#include <stdexcept>

#include <gl/glew.h>
#include <glm/glm.hpp>

#include <assimp/scene.h>

// ------------------------------------------------------------------------------------------- Mesh

class Mesh {
private:
	GLuint vbo, ebo;
	unsigned int elements_count;

	glm::vec4 color;
	GLuint texture;

	glm::vec3 min_point, max_point;

public:
	Mesh(const char* loading_path, const aiScene& ai_scene, aiMesh& ai_mesh);
	~Mesh();

	void get_aabb(glm::vec3& min_point, glm::vec3& max_point);

	void render(GLuint program);
};

// ------------------------------------------------------------------------------------------- Model

class Model {
private:
	std::vector<Mesh> meshes;

public:
	Model(const char* model_file_path);

	void get_aabb(glm::vec3& min_point, glm::vec3& max_point);

	void render(GLuint program);
};

// ------------------------------------------------------------------------------------------- Voxelizer

class Voxel_Grid {
public:
	const unsigned int width, height, depth;
	const std::unique_ptr<GLfloat[]> data;

	Voxel_Grid(unsigned int width, unsigned int height, unsigned int depth);
};

class Voxel_Visualizer {
private:
	GLuint vbo, ebo;
	unsigned int vertices_count;

public:
	Voxel_Visualizer();
	~Voxel_Visualizer();

	void render(GLuint program, Voxel_Grid& grid);
};

class Voxelizer {
private:
	GLuint framebuffer, texture3d, shader_program;

public:
	Voxelizer();
	~Voxelizer();

	void voxelize(Model model, Voxel_Grid& grid);
};

namespace Conversion {
	typedef enum {
		DEBUG_MODE = 0b0001
	} Settings_Flags;

	typedef struct {
		const uint8_t* model_path;
		uint8_t resolution_width, resolution_height, resolution_depth;
		uint8_t flags;
	} Settings;

	class Chain;

	/********************************************************** Step */

	class Step {
	public:
		virtual void act(Step& previous, Chain& chain) = 0;
	};

	class LoadModel_Step : Step {
	private:
		Model model;

	public:
		void act(Step& previous, Chain& chain) override;
	};

	class Voxelize_Step : Step {
		void act(Step& previous, Chain& chain) override;
	};

	class McBlocksAssociation_Step : Step {
		void act(Step& previous, Chain& chain) override;
	};

	class SchematicConversion_Step : Step {
		void act(Step& previous, Chain& chain) override;
	};

	/********************************************************** Visualizer */

	class Visualizer : Step {
		void act(Step& previous, Chain& chain) override;
	};

	class LoadModel_Visualizer : Visualizer {
	};

	class Voxelize_Visualizer : Visualizer {
	};

	class BlockAssociation_Visualizer : Visualizer {
	};

	/********************************************************** Chain */

	class Chain {
	private:
		Settings settings;
		std::vector<Step> steps;
		unsigned int current_step;

	public:
		Chain(Settings settings);

		Step& get_current_step();
		bool next_step();
	};
}
