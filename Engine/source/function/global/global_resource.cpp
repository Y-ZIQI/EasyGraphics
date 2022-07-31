#include "function/global/global_resource.h"

namespace Eagle
{
	RuntimeGlobalResource g_global_resource;

	void RuntimeGlobalResource::initialize()
	{
		m_shaders = std::make_shared<ShaderManager>();
		m_asset_manager = std::make_shared<AssetManager>();
		m_image_reader = std::make_shared<ImageReader>();
	}

	void RuntimeGlobalResource::cleanup()
	{
		m_shaders.reset();
		m_asset_manager.reset();
		m_image_reader.reset();
	}

	RuntimeGlobalSettings g_global_setting;
}