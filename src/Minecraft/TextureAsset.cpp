#include "TextureAsset.hpp"

#include <string>
#include <iostream>

TextureAsset::TextureAsset()
{
	glGenTextures(1, &this->textures);
	glGenTextures(1, &this->textures_averages);
}

GLuint TextureAsset::get_textures()
{
	return this->textures;
}

GLuint TextureAsset::get_textures_averages()
{
	return this->textures_averages;
}

iColor TextureAsset::color_average_rgba(GLchar* image_data, size_t image_size)
{
	uint32_t r = 0, g = 0, b = 0, a = 0;

	for (uint32_t i = 0; i < image_size * 4; i += 4)
	{
		r += image_data[i];
		g += image_data[i + 1];
		b += image_data[i + 2];
		a += image_data[i + 3];
	}

	return {
		(uint8_t) (r / (float_t) image_size),
		(uint8_t) (g / (float_t) image_size),
		(uint8_t) (b / (float_t) image_size),
		(uint8_t) (a / (float_t) image_size)
	};
}

uint32_t load_uint32_t(std::ifstream& file)
{
	uint8_t blocks_count[4];
	file.read((char*)&blocks_count, 4);
	return
		((uint32_t)blocks_count[0] << 8 * 3) |
		((uint32_t)blocks_count[1] << 8 * 2) |
		((uint32_t)blocks_count[2] << 8 * 1) |
		((uint32_t)blocks_count[3]);
}

void TextureAsset::load(std::ifstream& file)
{
	std::string mc_version;
	std::getline(file, mc_version, '\0');
	std::cout << "MC version: " << mc_version << std::endl;

	uint32_t blocks_count = load_uint32_t(file);
	std::cout << "Blocks count: " << blocks_count << std::endl;

	// textures init (16x16)
	glBindTexture(GL_TEXTURE_2D_ARRAY, this->textures);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 16, 16, blocks_count);

	// texture_averages init (1x1)
	glBindTexture(GL_TEXTURE_2D_ARRAY, this->textures_averages);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 1, 1, blocks_count);

	uint16_t layer = 0;

	GLchar image_pixels[256 * 4];
	while (blocks_count-- > 0)
	{
		// block type (1)
		uint8_t block_type;
		file.read((char*)&block_type, 1);

		// namespace id (minecraft:stone)
		std::string namespace_id;
		std::getline(file, namespace_id, '\0');

		// block data (0)
		uint8_t block_data;
		file.read((char*)&block_data, 1);

		// textures
		file.read(image_pixels, 256 * 4);

		glBindTexture(GL_TEXTURE_2D_ARRAY, this->textures);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, 16, 16, 1, GL_RGBA, GL_UNSIGNED_BYTE, image_pixels);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// averages
		iColor average = this->color_average_rgba(image_pixels, 256);
		GLchar average_pixels[4] = { average.r, average.g, average.b, average.a };

		glBindTexture(GL_TEXTURE_2D_ARRAY, this->textures_averages);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, average_pixels);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

		layer++;
	}
}
