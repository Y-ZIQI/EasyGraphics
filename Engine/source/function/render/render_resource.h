#pragma once

#include "function/render/rhi/rhi.h"
#include "function/render/render_type.h"

#include "function/world/scene.h"

#include "resource/res_type/data/mesh_data.h"
#include "resource/res_type/data/material.h"

#include <string>

namespace Eagle
{
	class RenderResource : public RHIRenderResource
	{
	public:
		RenderResource() {};
		~RenderResource() {};

		void initialize(RenderResourceInitInfo init_info);
		void cleanup();

		void uploadScene(std::shared_ptr<Scene> scene);

		RenderMeshData loadMeshData(const MeshData& source);
		std::shared_ptr<TextureData> loadTexture(const std::string& file, bool is_srgb, bool vflip);
		std::shared_ptr<TextureData> loadTexture(const std::string& directory, const std::string& file, bool is_srgb, bool vflip);
		RenderMaterialData loadMaterialData(const MaterialData& source, bool vflip);

		std::shared_ptr<Scene> m_current_scene;
	};
}