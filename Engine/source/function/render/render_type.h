#pragma once

#include "resource/res_type/formats.h"

#include <cstdint>
#include <memory>
#include <string>

namespace Eagle
{
    class BufferData
    {
    public:
        size_t m_size{ 0 };
        void* m_data{ nullptr };

        BufferData() = delete;
        BufferData(size_t size)
        {
            m_size = size;
            m_data = malloc(size);
        }
        ~BufferData()
        {
            if (m_data)
            {
                free(m_data);
            }
        }
        bool isValid() const { return m_data != nullptr; }
    };

    class TextureData
    {
    public:
        uint32_t m_width{ 0 };
        uint32_t m_height{ 0 };
        uint32_t m_depth{ 0 };
        uint32_t m_mip_levels{ 0 };
        uint32_t m_array_layers{ 0 };
        bool m_alpha{ false };
        void* m_pixels{ nullptr };
        uint32_t m_size{ 0 };

        EAGLE_DXGI_FORMAT m_format{ DXGI_FORMAT_UNKNOWN };
        EAGLE_IMAGE_TYPE   m_type{ EAGLE_IMAGE_TYPE_UNKNOWM };

        TextureData() = default;
        ~TextureData()
        {
            if (m_pixels)
            {
                free(m_pixels);
            }
        }
        bool isValid() const { return m_pixels != nullptr; }
    };

    struct MeshVertexDataDefinition
    {
        float x, y, z;    // position
        float nx, ny, nz; // normal
        float u, v;       // UV coordinates
        float tx, ty, tz; // tangent
        float btx, bty, btz; // bitangent
    };

    typedef uint32_t MeshIndexDataDefinition;

    struct StaticMeshData
    {
        std::shared_ptr<BufferData> m_vertex_buffer;
        std::shared_ptr<BufferData> m_index_buffer;
    };

    struct RenderMeshData
    {
        StaticMeshData m_static_mesh_data;
        //std::shared_ptr<BufferData> m_skeleton_binding_buffer;
    };

    struct RenderMaterialData
    {
        std::shared_ptr<TextureData> m_base_color_texture;
        //std::shared_ptr<TextureData> m_metallic_roughness_texture;
        std::shared_ptr<TextureData> m_specular_texture;
        std::shared_ptr<TextureData> m_normal_texture;
        std::shared_ptr<TextureData> m_occlusion_texture;
        std::shared_ptr<TextureData> m_emissive_texture;
    };

}