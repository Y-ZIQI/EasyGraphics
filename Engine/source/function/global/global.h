#pragma once

#include "function/render/window_system.h"
#include "function/render/render_system.h"

namespace Eagle
{
	class RuntimeGlobalContext
	{
	public:
		void initialize();

		void cleanup();

		std::shared_ptr<WindowSystem> m_window_system;
		std::shared_ptr<RenderSystem> m_render_system;
	};

	extern RuntimeGlobalContext g_global_context;
}