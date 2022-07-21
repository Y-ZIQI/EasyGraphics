#pragma once

#include "resource/shader/shader_manager.h"

#include <memory>

namespace Eagle
{
	class RuntimeGlobalResource
	{
	public:
		void initialize();

		void cleanup();

		// Global shaders
		std::shared_ptr<ShaderManager> m_shaders;
		// Global textures (like SMAA areaTex)
		// std::shared_ptr<TextureManager> m_textures;
	};

	extern RuntimeGlobalResource g_global_resource;

	class RuntimeGlobalSettings
	{
	public:
		int CSM_maps = 3;
		float CSM_distances[4] = { 0.0001f, 1.0, 3.0, 10.0 };
	};

	extern RuntimeGlobalSettings g_global_setting;
}