#include "function/global/global.h"

namespace Eagle
{
	RuntimeGlobalContext g_global_context;

	void RuntimeGlobalContext::initialize() {
		m_asset_manager = std::make_shared<AssetManager>();

		m_world_manager = std::make_shared<WorldManager>();

		WindowCreateInfo window_create_info;
		m_window_system = std::make_shared<WindowSystem>();
		m_window_system->initialize(window_create_info);

		RenderSystemInitInfo render_init_info;
		render_init_info.window_system = m_window_system;
		m_render_system = std::make_shared<RenderSystem>();
		m_render_system->initialize(render_init_info);

		m_input_system = std::make_shared<InputSystem>();
		m_input_system->initialize({ m_window_system, m_render_system, m_world_manager });
	}

	void RuntimeGlobalContext::cleanup() {
		m_asset_manager.reset();

		m_render_system->cleanup();
		m_render_system.reset();

		m_window_system->cleanup();
		m_window_system.reset();
	}
}