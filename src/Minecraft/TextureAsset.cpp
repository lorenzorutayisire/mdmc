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

iColor TextureAsset::color_average_rgba(char* image_data, size_t image_size)
{
	// https://sighack.com/post/averaging-rgb-colors-the-right-way

	uint32_t r = 0, g = 0, b = 0, a = 0;

	for (uint32_t i = 0; i < image_size * 4; i += 4)
	{
		uint8_t
			ur = image_data[i + 0],
			ug = image_data[i + 1],
			ub = image_data[i + 2],
			ua = image_data[i + 3];

		r += ur * ur;
		g += ug * ug;
		b += ub * ub;
		a += ua * ua;
	}

	return {
		(uint8_t) sqrt(r / image_size),
		(uint8_t) sqrt(g / image_size),
		(uint8_t) sqrt(b / image_size),
		(uint8_t) sqrt(a / image_size)
	};
}

uint32_t load_uint32_t(std::istream& file)
{
	uint8_t blocks_count[4];
	file.read((char*)&blocks_count, 4);
	return
		((uint32_t)blocks_count[0] << 8 * 3) |
		((uint32_t)blocks_count[1] << 8 * 2) |
		((uint32_t)blocks_count[2] << 8 * 1) |
		((uint32_t)blocks_count[3]);
}

void TextureAsset::load(std::istream& file)
{
	std::string mc_version;
	std::getline(file, mc_version, '\0'); // mc_version
	std::cout << "MC version: " << mc_version << std::endl;

	uint32_t blocks_count = load_uint32_t(file); // blocks_count
	std::cout << "Blocks count: " << blocks_count << std::endl;

	// textures init (16x16)
	glBindTexture(GL_TEXTURE_2D_ARRAY, this->textures);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 16, 16, blocks_count + 1);

	// texture_averages init (1x1)
	glBindTexture(GL_TEXTURE_2D_ARRAY, this->textures_averages);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 1, 1, blocks_count + 1);

	uint16_t layer = 1;

	while (file.peek() != EOF)
	{
		file.ignore(1); // block_type

		std::string namespaced_id;
		std::getline(file, namespaced_id, '\0'); // namespace_id

		std::cout << "ID: " << layer << " (" << namespaced_id << ")" << std::endl;

		file.ignore(1); // block_data

		char image_pixels[256 * 4];
		file.read(image_pixels, 16 * 16 * 4); // texture (16 * 16 * rgba)

		glBindTexture(GL_TEXTURE_2D_ARRAY, this->textures);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, 16, 16, 1, GL_RGBA, GL_UNSIGNED_BYTE, image_pixels);

		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// averages
		iColor average = this->color_average_rgba(image_pixels, 256);
		uint8_t average_pixels[4] = { average.r, average.g, average.b, average.a };

		glBindTexture(GL_TEXTURE_2D_ARRAY, this->textures_averages);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, average_pixels);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

		layer++;
	}
}
