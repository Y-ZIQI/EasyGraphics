#pragma once

#include "resource/asset/asset_manager.h"
#include "function/world/world_manager.h"
#include "function/input/input_system.h"
#include "function/render/window_system.h"
#include "function/render/render_system.h"

namespace Eagle
{
	class RuntimeGlobalContext
	{
	public:
		void initialize();

		void cleanup();

		std::shared_ptr<AssetManager>	m_asset_manager;
		std::shared_ptr<WorldManager>	m_world_manager;
		std::shared_ptr<WindowSystem>	m_window_system;
		std::shared_ptr<InputSystem>	m_input_system;
		std::shared_ptr<RenderSystem>	m_render_system;
	};

	extern RuntimeGlobalContext g_global_context;
}