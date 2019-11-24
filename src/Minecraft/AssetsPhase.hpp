#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <memory>

#include "Phase.hpp"
#include "PhaseManager.hpp"

#include "GL/Scene.hpp"
#include "GL/Shader.hpp"
#include "GL/Program.hpp"
#include "GL/Viewer.hpp"

#include "Assets.hpp"

namespace mdmc
{
	using namespace Minecraft;

	class AssetsPhase : public Phase
	{
	private:
		std::shared_ptr<const Assets> assets;

		Program program;
		Viewer viewer;

		size_t selected_block_id;
		bool just_selected;

	public:
		AssetsPhase(const Assets& assets);

		void on_update(PhaseManager* phase_manager, float delta);
		void on_render(PhaseManager* phase_manager);
	};
}
