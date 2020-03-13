#pragma once

#include <memory>
#include <filesystem>

#include "scene/scene.hpp"

class aiSceneLoader
{
public:
	static std::shared_ptr<Scene> load(const std::filesystem::path& path);
};

