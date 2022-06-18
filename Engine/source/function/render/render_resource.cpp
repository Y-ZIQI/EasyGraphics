#include "function/render/render_resource.h"

namespace Eagle
{
    /*const MeshData mesh = {
    {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
        //{-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f},
        //{0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f},
        //{0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
        //{-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
        //{-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f},
        //{0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
        //{0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f},
        //{-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f}
    },
    {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4
    }
    };

    const std::string texture_file = "../Engine/resources/textures/texture.jpg";

    const std::string model_path = "../Engine/resources/models/viking_room.obj";
    const std::string texture_path = "../Engine/resources/textures/viking_room.png";*/

    void RenderResource::uploadMeshRenderResource(std::shared_ptr<RHI> rhi, RenderMeshData mesh_data, RenderMaterialData material_data)
    {
        auto n_mesh = createMesh(mesh_data);
        m_render_meshes[0] = n_mesh;
        auto n_material = createPBRMaterial(material_data);
        m_render_materials[0] = n_material;

        m_material_meshes[0] = { 0 };
    }

    void RenderResource::loadObjWithTexture(std::shared_ptr<MeshData> mesh_data, const std::string tex_file)
    {
        RenderMeshData r_mesh_data;
        r_mesh_data = loadMeshData(*mesh_data);
        RenderMaterialData material_data;
        material_data = loadMaterialData(tex_file);
        uploadMeshRenderResource(m_rhi, r_mesh_data, material_data);
    }

}