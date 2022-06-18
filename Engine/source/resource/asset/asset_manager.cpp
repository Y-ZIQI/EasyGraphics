#include "resource/asset/asset_manager.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

namespace Eagle
{
	std::shared_ptr<MeshData> Importer::loadObj(const std::string& model_path)
	{
		std::shared_ptr<MeshData> n_mesh = std::make_shared<MeshData>();

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, model_path.c_str())) {
            throw std::runtime_error(warn + err);
        }

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        uint32_t indices = 0;
        for (const auto& shape : shapes) {
            indices += shape.mesh.indices.size();
        }
        n_mesh->index_buffer.resize(indices);

        uint32_t v_idx = 0, i_idx = 0;
        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex vertex{};

                vertex.pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                vertex.tex_coord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                };

                vertex.color = { 1.0f, 1.0f, 1.0f };

                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = v_idx++;
                }

                n_mesh->index_buffer[i_idx++] = uniqueVertices[vertex];
            }
        }

        n_mesh->vertex_buffer.resize(uniqueVertices.size());
        for (auto& v : uniqueVertices) {
            n_mesh->vertex_buffer[v.second] = v.first;
        }

		return n_mesh;
	}

	std::shared_ptr<MeshData> AssetManager::loadStaticMesh(const std::string& model_path)
	{
		return m_importer.loadObj(model_path);
	}
}