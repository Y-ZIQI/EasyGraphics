#pragma once

#include "resource/res_type/data/camera.h"
#include "resource/res_type/data/mesh_data.h"
#include "resource/res_type/data/material.h"
#include "resource/res_type/data/light_data.h"

#include <memory>
#include <vector>
#include <map>
#include <set>

namespace Eagle
{
	class Scene
	{
	public:
		void cleanup();

		std::string m_directory;
		bool m_texture_filp_vertically;

		uint32_t m_current_camera;
		std::vector<Camera> m_cameras;

		std::map<uint32_t, Transform> m_transforms;
		std::map<uint32_t, MeshData> m_meshes;
		std::map<uint32_t, MaterialData> m_materials;
		std::map<uint32_t, std::set<uint32_t>> m_material_meshes;

		DirectionalLight m_dir_light;

		Camera& getCamera();
	};
}