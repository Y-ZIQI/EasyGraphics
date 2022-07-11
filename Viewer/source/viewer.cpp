#include "include/viewer.h"

namespace Eagle
{
	Viewer::Viewer(){
		m_engine = std::make_shared<EagleEngine>();
		m_engine->initialize();

		g_global_context.m_world_manager->loadSceneJson("../Engine/resources/models/Arcade/Arcade.json", g_global_context.m_asset_manager);
		g_global_context.m_render_system->m_render_resource->uploadScene(
			g_global_context.m_world_manager->m_current_scene
		);

		/*g_global_context.m_world_manager->loadScene("../Engine/resources/models/Arcade/Arcade.fbx", g_global_context.m_asset_manager);
		g_global_context.m_render_system->m_render_resource->uploadScene(
			g_global_context.m_world_manager->m_current_scene
		);

		g_global_context.m_world_manager->loadObjSceneWithTexture("../Engine/resources/models/viking_room.obj", "../Engine/resources/textures/viking_room.png", g_global_context.m_asset_manager);
		g_global_context.m_render_system->m_render_resource->uploadScene(
			g_global_context.m_world_manager->m_current_scene
		);*/
	}

	Viewer::~Viewer(){
		m_engine->cleanup();
		m_engine.reset();
	}

	void Viewer::run() {
		m_engine->mainLoop();
	}
}