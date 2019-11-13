#pragma once

#include <GL/glew.h>
#include <fstream>

#include "../GL/Color.hpp"

#define MINECRAFT_TEXTURE_SIDE 16
#define MINECRAFT_TEXTURE_SIZE MINECRAFT_TEXTURE_SIDE * MINECRAFT_TEXTURE_SIDE

class TextureAsset
{
private:
	GLuint textures, textures_averages;

public:
	TextureAsset();

	iColor color_average_rgba(char* image_data, size_t image_size);

	GLuint get_textures();
	GLuint get_textures_averages();

	void load(std::istream& file);
};
