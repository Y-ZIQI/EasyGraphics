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

    void RenderResource::initialize(RenderResourceInitInfo init_info)
    {
        RHIRenderResource::initialize(init_info);
    }

    void RenderResource::cleanup()
    {
        RHIRenderResource::cleanup();
    }

    void RenderResource::uploadMeshRenderResource(RenderMeshData mesh_data, RenderMaterialData material_data)
    {
        auto n_mesh = createMesh(mesh_data);
        m_render_meshes[0] = n_mesh;
        auto n_material = createPBRMaterial(material_data);
        m_render_materials[0] = n_material;

        m_material_meshes[0] = { 0 };
    }

    void RenderResource::uploadScene(std::shared_ptr<Scene> scene)
    {
        m_current_scene = scene;
        for (auto& pair : scene->m_meshes) {
            auto& idx = pair.first;
            auto& mesh_data = pair.second;

            RenderMeshData r_mesh_data;
            r_mesh_data = loadMeshData(mesh_data);
            auto n_mesh = createMesh(r_mesh_data);
            m_render_meshes[idx] = n_mesh;
        }
        for (auto& pair : scene->m_materials) {
            auto& idx = pair.first;
            auto& material_data = pair.second;

            RenderMaterialData r_material_data;
            r_material_data = loadMaterialData(scene->m_directory + material_data.m_base_colour_texture_file, scene->m_texture_filp_vertically);
            auto n_material = createPBRMaterial(r_material_data);
            m_render_materials[idx] = n_material;
        }
        m_material_meshes = std::map<uint32_t, std::set<uint32_t>>(scene->m_material_meshes);
    }

    void RenderResource::loadObjWithTexture(std::shared_ptr<MeshData> mesh_data, const std::string tex_file)
    {
        RenderMeshData r_mesh_data;
        r_mesh_data = loadMeshData(*mesh_data);
        RenderMaterialData material_data;
        material_data = loadMaterialData(tex_file, false);
        uploadMeshRenderResource(r_mesh_data, material_data);
    }

}