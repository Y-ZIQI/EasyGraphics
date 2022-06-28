#include "include/viewer.h"

namespace Eagle
{
	Viewer::Viewer(){
		m_engine = std::make_shared<EagleEngine>();
		m_engine->initialize();

		g_global_context.m_world_manager->loadScene("../Engine/resources/models/Arcade/Arcade.fbx", g_global_context.m_asset_manager);
		g_global_context.m_render_system->m_render_resource->uploadScene(
			g_global_context.m_world_manager->m_current_scene
		);

		//auto mesh = g_global_context.m_asset_manager->loadStaticMesh("../Engine/resources/models/viking_room.obj");
		//const std::string tex_file = "../Engine/resources/textures/viking_room.png";
		//g_global_context.m_render_system->m_render_resource->loadObjWithTexture(mesh, tex_file);
	}

	Viewer::~Viewer(){
		m_engine->cleanup();
		m_engine.reset();
	}

	void Viewer::run() {
		m_engine->mainLoop();
	}
}