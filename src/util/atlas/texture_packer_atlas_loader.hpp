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
	static std::shared_ptr<const Atlas> load(const std::filesystem::path& texture_path, const std::filesystem::path& json_path);
};
