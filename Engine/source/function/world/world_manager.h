#pragma once

#include "resource/asset/asset_manager.h"
#include "function/world/scene.h"

#include <vector>
#include <map>
#include <set>

namespace Eagle
{
	class WorldManager
	{
	public:
		void loadScene(const std::string scene_path);
		void loadSceneJson(const std::string json_path);
		void loadObjSceneWithTexture(const std::string scene_path, const std::string texture_path);

		void tick();

		std::shared_ptr<Scene> m_current_scene;
	};
}