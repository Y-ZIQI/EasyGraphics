#include "engine.h"

namespace Eagle 
{
	void EagleEngine::initialize()
	{
		g_global_context.initialize();
	}

	void EagleEngine::mainLoop()
	{
		while (!g_global_context.m_window_system->windowShouldClose()) {
			g_global_context.m_window_system->pollEvents();
			g_global_context.m_render_system->tick();
		}
	}

	void EagleEngine::cleanup()
	{
		g_global_context.cleanup();
	}
}