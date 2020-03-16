#pragma once

#include <string>
#include <unordered_map>
#include <filesystem>

#include <glm/glm.hpp>

struct Atlas
{
	struct Texture
	{
		glm::vec2 from;
		glm::vec2 size;
	};

	std::filesystem::path image_path;
	glm::vec2 size;
	std::unordered_map<std::string, Texture const> texture_by_name;
};
