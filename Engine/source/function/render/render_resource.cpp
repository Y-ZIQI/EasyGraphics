#include "function/render/render_resource.h"

#include "function/global/global_resource.h"

namespace Eagle
{
    void RenderResource::initialize(RenderResourceInitInfo init_info)
    {
        RHIRenderResource::initialize(init_info);
    }

    void RenderResource::cleanup()
    {
        RHIRenderResource::cleanup();
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
            r_material_data = loadMaterialData(material_data, scene->m_texture_filp_vertically);
            auto n_material = createPBRMaterial(r_material_data);
            m_render_materials[idx] = n_material;
        }
        m_material_meshes = std::map<uint32_t, std::set<uint32_t>>(scene->m_material_meshes);
    }

    RenderMeshData RenderResource::loadMeshData(const MeshData& source)
    {
        RenderMeshData ret;

        // vertex buffer
        size_t vertex_size = source.vertex_buffer.size() * sizeof(MeshVertexDataDefinition);
        ret.m_static_mesh_data.m_vertex_buffer = std::make_shared<BufferData>(vertex_size);
        MeshVertexDataDefinition* vertex =
            (MeshVertexDataDefinition*)ret.m_static_mesh_data.m_vertex_buffer->m_data;
        for (size_t i = 0; i < source.vertex_buffer.size(); i++)
        {
            vertex[i].x = source.vertex_buffer[i].pos.x;
            vertex[i].y = source.vertex_buffer[i].pos.y;
            vertex[i].z = source.vertex_buffer[i].pos.z;
            vertex[i].nx = source.vertex_buffer[i].normal.x;
            vertex[i].ny = source.vertex_buffer[i].normal.y;
            vertex[i].nz = source.vertex_buffer[i].normal.z;
            vertex[i].u = source.vertex_buffer[i].tex_coord.x;
            vertex[i].v = source.vertex_buffer[i].tex_coord.y;
            vertex[i].tx = source.vertex_buffer[i].tangent.x;
            vertex[i].ty = source.vertex_buffer[i].tangent.y;
            vertex[i].tz = source.vertex_buffer[i].tangent.z;
            vertex[i].btx = source.vertex_buffer[i].bitangent.x;
            vertex[i].bty = source.vertex_buffer[i].bitangent.y;
            vertex[i].btz = source.vertex_buffer[i].bitangent.z;
        }

        // index buffer
        size_t index_size = source.index_buffer.size() * sizeof(MeshIndexDataDefinition);
        ret.m_static_mesh_data.m_index_buffer = std::make_shared<BufferData>(index_size);
        MeshIndexDataDefinition* index = (MeshIndexDataDefinition*)ret.m_static_mesh_data.m_index_buffer->m_data;
        for (size_t i = 0; i < source.index_buffer.size(); i++)
        {
            index[i] = static_cast<MeshIndexDataDefinition>(source.index_buffer[i]);
        }

        return ret;
    }

    std::shared_ptr<TextureData> RenderResource::loadTexture(const std::string& file, bool is_srgb, bool vflip)
    {
        std::shared_ptr<TextureData> texture = std::make_shared<TextureData>();

        auto image = g_global_resource.m_image_reader->readImage(file, is_srgb, vflip);
        if (!image.data)
            return nullptr;
        texture->m_pixels = image.data;
        texture->m_size = image.dataSize;
        texture->m_width = image.width;
        texture->m_height = image.height;
        //texture->m_format = (is_srgb) ? EAGLE_PIXEL_FORMAT::EAGLE_PIXEL_FORMAT_R8G8B8A8_SRGB :
        //    EAGLE_PIXEL_FORMAT::EAGLE_PIXEL_FORMAT_R8G8B8A8_UNORM; 
        texture->m_format = image.format;
        texture->m_depth = image.depth;
        texture->m_array_layers = image.arraySize;
        texture->m_mip_levels = image.numMips;
        texture->m_alpha = image.supportsAlpha;
        texture->m_type = image.type;

        return texture;
    }

    std::shared_ptr<TextureData> RenderResource::loadTexture(const std::string& directory, const std::string& file, bool is_srgb, bool vflip)
    {
        if (file != "")
            return loadTexture(directory + file, is_srgb, vflip);
        return nullptr;
    }

    RenderMaterialData RenderResource::loadMaterialData(const MaterialData& source, bool vflip)
    {
        RenderMaterialData ret;
        ret.m_base_color_texture = loadTexture(m_current_scene->m_directory, source.m_base_color_texture_file, true, vflip);
        ret.m_specular_texture = loadTexture(m_current_scene->m_directory, source.m_specular_texture_file, false, vflip);
        ret.m_normal_texture = loadTexture(m_current_scene->m_directory, source.m_normal_texture_file, false, vflip);
        ret.m_emissive_texture = loadTexture(m_current_scene->m_directory, source.m_emissive_texture_file, false, vflip);
        return ret;
    }

}