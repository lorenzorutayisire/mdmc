#pragma once

#include <string>
#include <unordered_map>

#include <GL/glew.h>

struct Atlas
{
	struct Texture
	{
		unsigned int x, y;
		unsigned int width, height;
	};

	GLuint name;
	unsigned int width, height;

	std::unordered_map<std::string, Texture> texture_by_name;

	Atlas();
	Atlas(const Atlas&) = delete;
	Atlas(const Atlas&&) = delete;

	~Atlas();
};
