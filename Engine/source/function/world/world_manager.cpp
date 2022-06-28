#include "function/world/world_manager.h"

namespace Eagle
{
	void WorldManager::loadScene(const std::string scene_path, std::shared_ptr<AssetManager> asset_manager)
	{
		m_current_scene = std::make_shared<Scene>();
		asset_manager->loadScene(scene_path, m_current_scene);

		Camera n_camera({
			glm::vec3(2.0f, 2.0f, 2.0f),
			glm::vec3(0.0f, 0.0f, 0.0f), 
			glm::vec3(0.0f, 0.0f, 1.0f),
			//glm::vec2(glm::radians(45.0f), m_rhi->m_swapchain_extent.width / (float)m_rhi->m_swapchain_extent.height),
			glm::vec2(glm::radians(45.0f), 1280 / (float)720),
			0.1f, 10.0f
		});
		m_current_scene->m_cameras.push_back(n_camera);
	}
}