#include "voxelizer.hpp"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/cimport.h>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// ------------------------------------------------------------------------------------------- Mesh

Mesh::Mesh(const char *loading_path, const aiScene& ai_scene, aiMesh& ai_mesh) {
	// VBO
	std::vector<GLfloat> vertices;

	min_point = glm::vec3(0.0f);
	max_point = glm::vec3(0.0f);

	glGenBuffers(1, &this->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	for (int i = 0; i < ai_mesh.mNumVertices; i++) {
		aiVector3D vertex = ai_mesh.mVertices[i];

		if (vertex.x < this->min_point.x) this->min_point.x = vertex.x;
		if (vertex.y < this->min_point.y) this->min_point.y = vertex.y;
		if (vertex.z < this->min_point.z) this->min_point.z = vertex.z;

		if (vertex.x > this->max_point.x) this->max_point.x = vertex.x;
		if (vertex.y > this->max_point.y) this->max_point.y = vertex.y;
		if (vertex.z > this->max_point.z) this->max_point.z = vertex.z;

		vertices.push_back(vertex.x);
		vertices.push_back(vertex.y);
		vertices.push_back(vertex.z);

		if (ai_mesh.HasTextureCoords(0)) {
			vertices.push_back(ai_mesh.mTextureCoords[0][i].x);
			vertices.push_back(ai_mesh.mTextureCoords[0][i].y);
		}
		else {
			vertices.push_back(0);
			vertices.push_back(0);
		}
	}

	// EBO
	std::vector<GLuint> indices;
	glGenBuffers(1, &this->ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
	for (int i = 0; i < ai_mesh.mNumFaces; i++) {
		if (ai_mesh.mFaces[i].mNumIndices != 3) {
			throw std::invalid_argument("A triangular face must have 3 indices.");
		}
		indices.push_back(ai_mesh.mFaces[i].mIndices[0]);
		indices.push_back(ai_mesh.mFaces[i].mIndices[1]);
		indices.push_back(ai_mesh.mFaces[i].mIndices[2]);
	}
	this->elements_count = indices.size();

	// Material
	if (ai_mesh.mMaterialIndex >= 0 && ai_mesh.mMaterialIndex < ai_scene.mNumMaterials) {
		aiMaterial* ai_mat = ai_scene.mMaterials[ai_mesh.mMaterialIndex];
		aiColor4D ai_col;
		aiString ai_path;

		aiGetMaterialColor(ai_mat, AI_MATKEY_COLOR_DIFFUSE, &ai_col);
		this->color = glm::vec4(ai_col.r, ai_col.g, ai_col.b, ai_col.a);

		aiGetMaterialTexture(ai_mat, aiTextureType_DIFFUSE, 0, &ai_path);

		int width, height;
		std::string tex_path = std::string(loading_path) + "/" + std::string(ai_path.C_Str());
		uint8_t *image = stbi_load(tex_path.c_str(), &width, &height, NULL, STBI_rgb_alpha);

		glGenTextures(1, &this->texture);
		glBindTexture(GL_TEXTURE_2D, this->texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

		stbi_image_free(image);
	}
	else {
		this->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		this->texture = NULL;
	}
}

Mesh::~Mesh() {
	glDeleteBuffers(1, &this->vbo);
	glDeleteBuffers(1, &this->ebo);
	glDeleteTextures(1, &this->texture);
}

void Mesh::get_aabb(glm::vec3& min_point, glm::vec3& max_point) {
	this->min_point = min_point;
	this->max_point = max_point;
}

void Mesh::render(GLuint program) {
	if (this->texture != NULL) {
		glBindTexture(GL_ARRAY_BUFFER, this->texture);
	}

	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 5 * sizeof(GLfloat), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 5 * sizeof(GLfloat), 0);

	glDrawElements(GL_TRIANGLES, this->elements_count, GL_UNSIGNED_INT, 0);
}

// ------------------------------------------------------------------------------------------- Model

Model::Model(const char *model_file_path) {
	const aiScene *scene = aiImportFile(model_file_path, aiProcess_Triangulate);
	if (scene == NULL) {
		throw std::invalid_argument("Can't load scene from the given path.");
	}

	std::string parent_path =
		std::string(model_file_path).substr(0, std::string(model_file_path).find_last_of("/\\"));

	for (int i = 0; i < scene->mNumMeshes; i++) {
		this->meshes.push_back(Mesh(parent_path.c_str(), *scene, *scene->mMeshes[i]));
	}
}

void Model::get_aabb(glm::vec3& min_point, glm::vec3& max_point) {
	min_point = glm::vec3(0.0f);
	max_point = glm::vec3(0.0f);

	for (int i = 0; i < this->meshes.size(); i++) {
		glm::vec3 mesh_min, mesh_max;
		this->meshes[i].get_aabb(mesh_min, mesh_max);

		if (mesh_min.x < min_point.x) min_point.x = mesh_min.x;
		if (mesh_min.y < min_point.y) min_point.y = mesh_min.y;
		if (mesh_min.z < min_point.z) min_point.z = mesh_min.z;

		if (mesh_max.x > max_point.x) max_point.x = mesh_max.x;
		if (mesh_max.y > max_point.y) max_point.y = mesh_max.y;
		if (mesh_max.z > max_point.z) max_point.z = mesh_max.z;
	}
}

void Model::render(GLuint program) {
	for (int i = 0; i < this->meshes.size(); i++) {
		this->meshes[i].render(program);
	}
}

const GLchar* VOXELIZER_VERTEX_SHADER_SRC = R"glsl(
	#version 450

	in vec3 position;
	in vec2 texture_coords;

	out vec2 Texture_coords;

	uniform mat4 transform;
	uniform mat4 camera;

	void main() {
		gl_Position = camera * transform * vec4(position, 1.0);
		Texture_coords = texture_coords;
	}
)glsl";

const GLchar* VOXELIZER_FRAGMENT_SHADER_SRC = R"glsl(
	#version 450

	in vec2 Texture_coords;
	
	uniform vec4 color;
	uniform sampler2D diffuse_texture;

	void main() {
		gl_FragColor = color * texture(diffuse_texture, Texture_coords);
	}
)glsl";

/**************************************************************
	Voxel_Grid
	Stores somehow the voxel data retrieved from the Voxelizer.
*/

Voxel_Grid::Voxel_Grid(unsigned int width, unsigned int height, unsigned int depth) :
	width(width),
	height(height),
	depth(depth),
	data(std::make_unique<GLfloat[]>(width * height * depth * 4)) {
}

/**************************************************************
	Voxel_Visualizer
	Visualizes a given Voxel_Grid.
*/

Voxel_Visualizer::Voxel_Visualizer() {
	glGenBuffers(1, &this->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	GLfloat vertices[] = {
		// Front
		0, 0, 1,
		0, 1, 1,
		1, 1, 1,
		1, 0, 1,

		// Back
		0, 0, 0,
		0, 1, 0,
		1, 1, 0,
		1, 0, 0,

		// Right
		1, 0, 0,
		1, 1, 0,
		1, 1, 1,
		1, 0, 1,

		// Left
		0, 0, 0,
		0, 1, 0,
		0, 1, 1,
		0, 0, 1,

		// Top
		0, 1, 0,
		1, 1, 0,
		1, 1, 1,
		0, 1, 1,

		// Bottom
		0, 0, 0,
		1, 0, 0,
		1, 0, 1,
		0, 0, 1
	};
	this->vertices_count = sizeof(vertices) / sizeof(GLfloat);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

Voxel_Visualizer::~Voxel_Visualizer() {
	glDeleteBuffers(1, &this->vbo);
}

void Voxel_Visualizer::render(GLuint program, Voxel_Grid& grid) {
	for (unsigned int x = 0; x < grid.width; x++) {
		for (unsigned int y = 0; y < grid.height; y++) {
			for (unsigned int z = 0; z < grid.depth; z++) {
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
				GLuint uni_loc;

				uni_loc = glGetUniformLocation(program, "transform");
				glUniformMatrix4fv(uni_loc, 1, false, glm::value_ptr(transform));

				uni_loc = glGetUniformLocation(program, "camera");
				glUniformMatrix4fv(uni_loc, 1, false, glm::value_ptr(glm::mat4(1.0f)));

				glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
				glDrawArrays(GL_FLOAT, 0, this->vertices_count);
			}
		}
	}
}

/**************************************************************
	Voxelizer
	The voxelizer converts a given Model to a Voxel_Grid.
*/

GLuint load_shader(GLenum shader_type, const GLchar *shader_src) {
	GLuint shader = glCreateShader(shader_type);
	glShaderSource(shader, 1, &shader_src, NULL);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		char info_log[2048];
		glGetShaderInfoLog(shader, 2048, NULL, &info_log[0]);
		throw std::runtime_error(info_log);
	}
}

Voxelizer::Voxelizer() {
	// Framebuffer
	glGenFramebuffers(1, &this->framebuffer);

	// Texture
	glGenTextures(1, &this->texture3d);

	// Program
	this->shader_program = glCreateProgram();

	GLuint shader;
	shader = load_shader(GL_VERTEX_SHADER, VOXELIZER_VERTEX_SHADER_SRC);
	glAttachShader(this->shader_program, shader);

	shader = load_shader(GL_FRAGMENT_SHADER, VOXELIZER_FRAGMENT_SHADER_SRC);
	glAttachShader(this->shader_program, shader);

	glLinkProgram(this->shader_program);
}

Voxelizer::~Voxelizer() {
	glDeleteFramebuffers(1, &this->framebuffer);
	glDeleteTextures(1, &this->texture3d);
	glDeleteProgram(this->shader_program);
}

void Voxelizer::voxelize(Model model, Voxel_Grid& grid) {
	glUseProgram(this->shader_program);

	glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer);
	glViewport(0, 0, grid.width, grid.height);

	glBindTexture(GL_TEXTURE_3D, this->texture3d);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, grid.width, grid.height, grid.depth, 0, GL_RGBA, GL_FLOAT, NULL);

	GLuint uni_loc;

	// Transform
	glm::vec3 min_point, max_point;
	model.get_aabb(min_point, max_point);
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), -min_point);
	transform = glm::scale(transform, 1.0f / (max_point - min_point));
	uni_loc = glGetUniformLocation(this->shader_program, "transform");
	glUniformMatrix4fv(uni_loc, 1, false, glm::value_ptr(transform));

	float slice_size = 1.0f / (float) grid.depth;

	for (int slice = 0; slice < grid.depth; slice++) {
		glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, this->texture3d, 0, slice);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "Framebuffer is not ready." << std::endl;
			throw;
		}

		// Camera
		float near_plane = -(slice * slice_size);
		float far_plane = near_plane - slice_size;
		glm::mat4 camera = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, near_plane, far_plane);
		uni_loc = glGetUniformLocation(this->shader_program, "camera");
		glUniformMatrix4fv(uni_loc, 1, false, glm::value_ptr(camera));

		glClear(GL_COLOR_BUFFER_BIT);
		model.render(this->shader_program);
	}


	// Result
	glGetTexImage(GL_TEXTURE_3D, 0, GL_RGBA, GL_FLOAT, grid.data.get());
}

using namespace Conversion;

/********************************************************** Step */

void LoadModel_Step::act(Step& previous, Chain& chain) {
}

void Voxelize_Step::act(Step& previous, Chain& chain) {
	// TODO
}

void McBlocksAssociation_Step::act(Step& previous, Chain& chain) {
	// TODO
}

void SchematicConversion_Step::act(Step& previous, Chain& chain) {
	// TODO
}

/********************************************************** Visualizer */

/********************************************************** Chain */

Chain::Chain(Settings settings) : settings(settings), current_step(0) {
}
