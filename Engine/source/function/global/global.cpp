#include "function/global/global.h"

namespace Eagle
{
	RuntimeGlobalContext g_global_context;

	void RuntimeGlobalContext::initialize() {
		m_asset_manager = std::make_shared<AssetManager>();

		WindowCreateInfo window_create_info;
		m_window_system = std::make_shared<WindowSystem>();
		m_window_system->initialize(window_create_info);

		RenderSystemInitInfo render_init_info;
		render_init_info.window_system = m_window_system;
		m_render_system = std::make_shared<RenderSystem>();
		m_render_system->initialize(render_init_info);

		/*********/
		auto mesh = m_asset_manager->loadStaticMesh("../Engine/resources/models/viking_room.obj");
		const std::string tex_file = "../Engine/resources/textures/viking_room.png";
		m_render_system->m_render_resource->loadObjWithTexture(mesh, tex_file);
		/*********/
	}

	void RuntimeGlobalContext::cleanup() {
		m_asset_manager.reset();

		m_render_system->cleanup();
		m_render_system.reset();

		m_window_system->cleanup();
		m_window_system.reset();
	}

}