#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <unordered_map>

#include <GL/glew.h>

#include "util/atlas/atlas.hpp"

class TexturePackerAtlasLoader
{
public:
	static std::shared_ptr<Atlas const> load(std::filesystem::path const& image_path, std::filesystem::path const& descriptor_path);
};
