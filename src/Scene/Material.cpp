#include "Material.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>
#include <utility>

Material::Material() : color(1.0f, 1.0f, 1.0f, 1.0f) // Default color
{
	glGenTextures(1, &this->texture);

	// Default texture
	GLfloat emptyImage[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glBindTexture(GL_TEXTURE_2D, this->texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_FLOAT, emptyImage);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Material::~Material()
{
}

void Material::loadAiMaterial(std::string loadingPath, aiMaterial* aiMaterial)
{
	// Texture
	aiString aiPath;
	aiReturn ai_return = aiGetMaterialTexture(aiMaterial, aiTextureType_DIFFUSE, 0, &aiPath);
	if (ai_return == aiReturn_SUCCESS && aiPath.length > 0)
	{
		int width, height;

		std::string path = std::string(loadingPath) + "/" + std::string(aiPath.C_Str());

		std::cout << "Image: " << path << std::endl;

		uint8_t* image = stbi_load(path.c_str(), &width, &height, NULL, STBI_rgb_alpha);
		
		if (!image)
		{
			std::cout << "Error during image loading: " << stbi_failure_reason() << std::endl;
		}

		// Delets the old default texture.
		glDeleteTextures(1, &this->texture);

		// Generates a new texture to store the loaded image.
		glGenTextures(1, &this->texture);

		glBindTexture(GL_TEXTURE_2D, this->texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(image);
	}

	// Color
	if (aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_DIFFUSE, &color) != aiReturn_SUCCESS)
	{
		this->color = aiColor4D(1.0f, 1.0f, 1.0f, 1.0f);
	}

	printf("Color: %.2f %.2f %.2f\n", this->color.r, this->color.g, this->color.b);
}

void Material::bind()
{
	glBindTexture(GL_TEXTURE_2D, this->texture); // Texture
	glUniform4f(2, this->color.r, this->color.g, this->color.b, this->color.a); // Color
}
