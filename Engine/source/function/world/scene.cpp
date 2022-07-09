#include "function/world/scene.h"

namespace Eagle
{
	void Scene::cleanup()
	{
		m_cameras.clear();
		m_transforms.clear();
		m_meshes.clear();
		m_materials.clear();
		m_material_meshes.clear();
	}

	Camera& Scene::getCamera()
	{
		return m_cameras[m_current_camera];
	}
}