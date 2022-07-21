#include "function/global/global_resource.h"

namespace Eagle
{
	RuntimeGlobalResource g_global_resource;

	void RuntimeGlobalResource::initialize()
	{
		m_shaders = std::make_shared<ShaderManager>();
	}

	void RuntimeGlobalResource::cleanup()
	{
		m_shaders.reset();
	}

	RuntimeGlobalSettings g_global_setting;
}