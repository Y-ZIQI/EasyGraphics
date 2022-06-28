#pragma once

#include "function/render/rhi/rhi.h"
#include "function/render/render_type.h"

#include "function/world/scene.h"

#include "resource/res_type/data/mesh_data.h"

#include <string>

namespace Eagle
{
	//extern const MeshData mesh;
	//extern const std::string texture_file;
	//extern const std::string model_path;
	//extern const std::string texture_path;

	class RenderResource : public RHIRenderResource
	{
	public:
		RenderResource() {};
		~RenderResource() {};

		void initialize(RenderResourceInitInfo init_info);
		void cleanup();

		void uploadMeshRenderResource(RenderMeshData mesh_data, RenderMaterialData material_data);
		void uploadScene(std::shared_ptr<Scene> scene);

		void loadObjWithTexture(std::shared_ptr<MeshData> mesh_data, const std::string tex_file);

		std::shared_ptr<Scene> m_current_scene;
	};
}