#pragma once

#include "resource/res_type/data/mesh_data.h"

#include <assimp/Importer.hpp>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <stdexcept>

namespace Eagle
{
	class Importer {
	public:
		Importer() {};
		~Importer() {};

		std::shared_ptr<MeshData> loadObj(const std::string& model_path);
	};

	class AssetManager {
	public:
		AssetManager() {};
		~AssetManager() {};

		std::shared_ptr<MeshData> loadStaticMesh(const std::string& model_path);

	private:
		Importer m_importer;
	};
}

namespace std {
	template<> struct hash<Eagle::Vertex> {
		size_t operator()(Eagle::Vertex const& vertex) const {
			return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.tex_coord) << 1);
		}
	};
}