#pragma once

#include "function/render/rhi/rhi.h"
#include "function/render/render_type.h"

#include "resource/res_type/data/mesh_data.h"

#include <string>

namespace Eagle
{
	extern const MeshData mesh;
	extern const std::string texture_file;

	class RenderResource : public RHIRenderResource
	{
	public:
		RenderResource() {};
		~RenderResource() {};

		void uploadMeshRenderResoucre(std::shared_ptr<RHI> rhi, RenderMeshData mesh_data, RenderMaterialData material_data);

	};
}