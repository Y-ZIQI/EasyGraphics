#include "resource/asset/asset_manager.h"

#include "platform/file_system/file_system.h"

#include <json11/json11.hpp>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

namespace Eagle
{
    glm::mat4 aiCast(const aiMatrix4x4& aiMat)
    {
        glm::mat4 glmMat;
        glmMat[0][0] = aiMat.a1; glmMat[0][1] = aiMat.a2; glmMat[0][2] = aiMat.a3; glmMat[0][3] = aiMat.a4;
        glmMat[1][0] = aiMat.b1; glmMat[1][1] = aiMat.b2; glmMat[1][2] = aiMat.b3; glmMat[1][3] = aiMat.b4;
        glmMat[2][0] = aiMat.c1; glmMat[2][1] = aiMat.c2; glmMat[2][2] = aiMat.c3; glmMat[2][3] = aiMat.c4;
        glmMat[3][0] = aiMat.d1; glmMat[3][1] = aiMat.d2; glmMat[3][2] = aiMat.d3; glmMat[3][3] = aiMat.d4;
        return glmMat;
    }

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
                    attrib.texcoords[2 * index.texcoord_index + 1]
                };

                // TODO: obj file normal
                vertex.normal = { 1.0f, 1.0f, 1.0f };
                vertex.tangent = { 1.0f, 1.0f, 1.0f };
                vertex.bitangent = { 1.0f, 1.0f, 1.0f };

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

    void Importer::loadFbxScene(const std::string& scene_path, std::shared_ptr<Scene> n_scene)
    {
        n_scene->cleanup();
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(scene_path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }
        n_scene->m_directory = scene_path.substr(0, scene_path.find_last_of('/') + 1);
        n_scene->m_texture_filp_vertically = true;

        // Add Materials
        for (uint32_t i = 0; i < scene->mNumMaterials; i++) {
            aiMaterial* mat = scene->mMaterials[i];
            aiString name_str, diffuse_str, specular_str, normal_str, emmissive_str;
            mat->Get(AI_MATKEY_NAME, name_str);
            mat->GetTexture(aiTextureType_DIFFUSE, 0, &diffuse_str);
            mat->GetTexture(aiTextureType_SPECULAR, 0, &specular_str);
            mat->GetTexture(aiTextureType_NORMALS, 0, &normal_str);
            mat->GetTexture(aiTextureType_EMISSIVE, 0, &emmissive_str);

            MaterialData n_mat;
            n_mat.m_material_name = name_str.C_Str();
            n_mat.m_base_color_texture_file = diffuse_str.C_Str();
            n_mat.m_specular_texture_file = specular_str.C_Str();
            n_mat.m_normal_texture_file = normal_str.C_Str();
            n_mat.m_emissive_texture_file = emmissive_str.C_Str();
            n_scene->m_materials[i] = n_mat;
            n_scene->m_material_meshes[i] = std::set<uint32_t>();
        }
        // Add Meshes
        for (uint32_t i = 0; i < scene->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[i];
            uint32_t vertices_i = mesh->mNumVertices, indices_i = 0, i_index = 0;
            glm::vec3 minB{ mesh->mVertices[0].x, mesh->mVertices[0].y, mesh->mVertices[0].z };
            glm::vec3 maxB{ mesh->mVertices[0].x, mesh->mVertices[0].y, mesh->mVertices[0].z };
            MeshData n_mesh;
            n_mesh.vertex_buffer.resize(vertices_i);
            for (uint32_t j = 0; j < vertices_i; j++) {
                Vertex vertex;
                vertex.pos = glm::vec3(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z);
                if (mesh->HasNormals())
                    vertex.normal = glm::vec3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z);
                if (mesh->mTextureCoords[0]) {
                    vertex.tex_coord = glm::vec2(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y);
                    vertex.tangent = glm::vec3(mesh->mTangents[j].x, mesh->mTangents[j].y, mesh->mTangents[j].z);
                    vertex.bitangent = glm::vec3(mesh->mBitangents[j].x, mesh->mBitangents[j].y, mesh->mBitangents[j].z);
                }
                else
                    vertex.tex_coord = glm::vec2(0.0f, 0.0f);
                minB = glm::min(minB, vertex.pos);
                maxB = glm::max(maxB, vertex.pos);
                n_mesh.vertex_buffer[j] = vertex;
            }
            for (uint32_t j = 0; j < mesh->mNumFaces; j++) {
                indices_i += mesh->mFaces[j].mNumIndices;
            }
            n_mesh.index_buffer.resize(indices_i);
            for (uint32_t j = 0; j < mesh->mNumFaces; j++) {
                aiFace face = mesh->mFaces[j];
                for (uint32_t k = 0; k < face.mNumIndices; k++)
                    n_mesh.index_buffer[i_index++] = face.mIndices[k];
            }
            n_mesh.setAABB(minB, maxB);
            n_scene->m_meshes[i] = n_mesh;
            n_scene->m_material_meshes[mesh->mMaterialIndex].insert(i);
        }
        processNode(scene->mRootNode, glm::mat4(1.0f), n_scene);
    }

    void Importer::processNode(aiNode* node, glm::mat4 transform, std::shared_ptr<Scene> n_scene)
    {
        glm::mat4 curr_transform = aiCast(node->mTransformation);
        glm::mat4 total_transform = curr_transform * transform;
        for (uint32_t i = 0; i < node->mNumMeshes; i++)
        {
            uint32_t meshID = node->mMeshes[i];
            n_scene->m_transforms[meshID].transform = glm::transpose(total_transform);
            n_scene->m_transforms[meshID].n_transform = glm::inverse(glm::mat3(total_transform));
        }
        for (uint32_t i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], total_transform, n_scene);
        }
    }

	std::shared_ptr<MeshData> AssetManager::loadStaticMesh(const std::string& model_path)
	{
		return m_importer.loadObj(model_path);
	}

    void AssetManager::loadScene(const std::string& scene_path, const std::string& texture_path, std::shared_ptr<Scene> n_scene)
    {
        n_scene->cleanup();
        //n_scene->m_directory = scene_path.substr(0, scene_path.find_last_of('/') + 1);
        n_scene->m_directory = std::string("");
        n_scene->m_texture_filp_vertically = true;
        MaterialData n_mat;
        n_mat.m_material_name = texture_path;
        n_mat.m_base_color_texture_file = texture_path;
        n_mat.m_specular_texture_file = std::string("");
        n_mat.m_normal_texture_file = std::string("");
        n_mat.m_emissive_texture_file = std::string("");
        n_scene->m_materials[0] = n_mat;
        auto mesh_ptr = loadStaticMesh(scene_path);
        n_scene->m_meshes[0] = *mesh_ptr;
        n_scene->m_material_meshes[0] = { 0 };
        n_scene->m_transforms[0] = { glm::mat4(1.0f), glm::mat3(1.0f) };
    }

    void AssetManager::loadScene(const std::string& scene_path, std::shared_ptr<Scene> n_scene)
    {
        m_importer.loadFbxScene(scene_path, n_scene);
    }

    void AssetManager::loadSceneJson(const std::string& json_path, std::shared_ptr<Scene> n_scene){
        std::string str = FileSystem::readFileToString(json_path);

        std::string err;
        json11::Json json_obj = json11::Json::parse(str, err);

        std::string scene_path = json_obj["models"][0]["file"].string_value();
        scene_path = json_path.substr(0, json_path.find_last_of('/') + 1) + scene_path;
        m_importer.loadFbxScene(scene_path, n_scene);

        auto& dir_light = json_obj["lights"][0];
        auto it = dir_light["intensity"].array_items();
        n_scene->m_dir_light.intensity = { it[0].number_value(), it[1].number_value(), it[2].number_value() };
        auto dir = dir_light["direction"].array_items();
        n_scene->m_dir_light.direction = { dir[0].number_value(), dir[1].number_value(), dir[2].number_value() };
        n_scene->m_dir_light.ambient = dir_light["ambient"].number_value();
    }
}