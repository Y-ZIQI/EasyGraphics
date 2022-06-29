#pragma once

#include "resource/asset/asset_manager.h"
#include "function/world/scene.h"

namespace Eagle
{
	class WorldManager
	{
	public:
		void loadScene(const std::string scene_path, std::shared_ptr<AssetManager> asset_manager);
		void loadObjSceneWithTexture(const std::string scene_path, const std::string texture_path, std::shared_ptr<AssetManager> asset_manager);

		std::shared_ptr<Scene> m_current_scene;
	};
}