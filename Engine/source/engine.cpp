#include "engine.h"

namespace Eagle 
{
	void EagleEngine::initialize()
	{
		g_global_resource.initialize();
		g_global_context.initialize();
	}

	void EagleEngine::mainLoop()
	{
		while (!g_global_context.m_window_system->windowShouldClose()) {
			g_global_context.m_window_system->pollEvents();

			float delta_time = g_global_context.m_window_system->getDeltaTime();
			g_global_context.m_world_manager->tick();
			g_global_context.m_input_system->tick(delta_time);
			g_global_context.m_render_system->tick();
		}
	}

	void EagleEngine::cleanup()
	{
		g_global_resource.cleanup();
		g_global_context.cleanup();
	}
}