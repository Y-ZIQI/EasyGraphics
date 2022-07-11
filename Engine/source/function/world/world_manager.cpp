#include "function/world/world_manager.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

namespace Eagle
{
	void WorldManager::loadScene(const std::string scene_path, std::shared_ptr<AssetManager> asset_manager)
	{
		m_current_scene = std::make_shared<Scene>();
		asset_manager->loadScene(scene_path, m_current_scene);

		Camera n_camera({
			glm::vec3(0.0f, 0.5f, 1.0f),
			//glm::vec2(glm::radians(45.0f), m_rhi->m_swapchain_extent.width / (float)m_rhi->m_swapchain_extent.height),
			glm::vec2(glm::radians(45.0f), 1280 / (float)720),
			0.0f, 0.0f,
			0.1f, 10.0f
		});
		m_current_scene->m_cameras.push_back(n_camera);
		m_current_scene->m_current_camera = 0;
	}

	void WorldManager::loadSceneJson(const std::string json_path, std::shared_ptr<AssetManager> asset_manager)
	{
		m_current_scene = std::make_shared<Scene>();
		asset_manager->loadSceneJson(json_path, m_current_scene);

		Camera n_camera({
			glm::vec3(0.0f, 0.5f, 1.0f),
			//glm::vec2(glm::radians(45.0f), m_rhi->m_swapchain_extent.width / (float)m_rhi->m_swapchain_extent.height),
			glm::vec2(glm::radians(45.0f), 1280 / (float)720),
			0.0f, 0.0f,
			0.1f, 10.0f
		});
		m_current_scene->m_cameras.push_back(n_camera);
		m_current_scene->m_current_camera = 0;
	}

	void WorldManager::loadObjSceneWithTexture(const std::string scene_path, const std::string texture_path, std::shared_ptr<AssetManager> asset_manager)
	{
		m_current_scene = std::make_shared<Scene>();
		asset_manager->loadScene(scene_path, texture_path, m_current_scene);

		Camera n_camera({
			glm::vec3(2.0f, 2.0f, 2.0f),
			//glm::vec2(glm::radians(45.0f), m_rhi->m_swapchain_extent.width / (float)m_rhi->m_swapchain_extent.height),
			glm::vec2(glm::radians(45.0f), 1280 / (float)720),
			0.0f, 0.0f,
			0.1f, 10.0f
			});
		m_current_scene->m_cameras.push_back(n_camera);
		m_current_scene->m_current_camera = 0;
	}

	void WorldManager::tick()
	{
		// TODO: need update
		//static auto startTime = std::chrono::high_resolution_clock::now();
		//auto currentTime = std::chrono::high_resolution_clock::now();
		//float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
		//auto model_matrix = glm::rotate(glm::mat4(1.0f), 1.0f * time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		// ---
		//for (auto& pair : m_current_scene->m_transforms) {
		//	pair.second.transform = model_matrix;			
		//	pair.second.n_transform = glm::transpose(glm::inverse(glm::mat3(model_matrix)));
		//}
	}
}