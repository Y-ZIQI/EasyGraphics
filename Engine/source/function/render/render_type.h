#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace Eagle
{
    enum class EAGLE_PIXEL_FORMAT : uint8_t
    {
        EAGLE_PIXEL_FORMAT_UNKNOWN = 0,
        EAGLE_PIXEL_FORMAT_R8G8B8_UNORM,
        EAGLE_PIXEL_FORMAT_R8G8B8_SRGB,
        EAGLE_PIXEL_FORMAT_R8G8B8A8_UNORM,
        EAGLE_PIXEL_FORMAT_R8G8B8A8_SRGB,
        EAGLE_PIXEL_FORMAT_R32G32_FLOAT,
        EAGLE_PIXEL_FORMAT_R32G32B32_FLOAT,
        EAGLE_PIXEL_FORMAT_R32G32B32A32_FLOAT
    };

    enum class EAGLE_IMAGE_TYPE : uint8_t
    {
        EAGLE_IMAGE_TYPE_UNKNOWM = 0,
        EAGLE_IMAGE_TYPE_2D
    };

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
        void* m_pixels{ nullptr };

        EAGLE_PIXEL_FORMAT m_format{ EAGLE_PIXEL_FORMAT::EAGLE_PIXEL_FORMAT_UNKNOWN };
        EAGLE_IMAGE_TYPE   m_type{ EAGLE_IMAGE_TYPE::EAGLE_IMAGE_TYPE_UNKNOWM };

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
        float r, g, b;    // color: may deleted
        float u, v;       // UV coordinates

        //float nx, ny, nz; // normal
        //float tx, ty, tz; // tangent
    };

    typedef uint16_t MeshIndexDataDefinition;

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
        //std::shared_ptr<TextureData> m_normal_texture;
        //std::shared_ptr<TextureData> m_occlusion_texture;
        //std::shared_ptr<TextureData> m_emissive_texture;
    };

}