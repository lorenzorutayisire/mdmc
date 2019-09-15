#include "Material.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Material::Material() : color(1.0f, 1.0f, 1.0f, 1.0f) // Default color
{
	glGenTextures(1, &this->texture);

	// Default texture
	GLfloat emptyImage[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glBindTexture(GL_TEXTURE_2D, this->texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_FLOAT, emptyImage);
}

Material::~Material()
{
	glDeleteTextures(1, &this->texture);
}

void Material::loadAiMaterial(aiMaterial* aiMaterial)
{
	// Texture
	aiString path;
	if (aiGetMaterialTexture(aiMaterial, aiTextureType_DIFFUSE, 0, &path) == aiReturn_SUCCESS && path.length > 0)
	{
		int width, height;
		uint8_t* image = stbi_load(path.C_Str(), &width, &height, NULL, STBI_rgb_alpha);
		
		glBindTexture(GL_TEXTURE_2D, this->texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	}

	// Color
	if (aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_DIFFUSE, &color) != aiReturn_SUCCESS)
	{
		this->color = aiColor4D(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

void Material::bind()
{
	glBindTexture(GL_TEXTURE_2D, this->texture); // Texture
	glUniform4f(0, this->color.r, this->color.g, this->color.b, this->color.a); // Color
}
