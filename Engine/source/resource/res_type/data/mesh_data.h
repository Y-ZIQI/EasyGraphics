#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <vector>
#include <string>

namespace Eagle
{
	class GVertex
	{
	public:
		float x, y, z;
		float r, g, b;
		float u, v;

		bool operator==(const GVertex& other) const {
			return x == other.x && y == other.y && z == other.z && r == other.r && g == other.g && b == other.b && u == other.u && v == other.v;
		}
	};

	class Vertex
	{
	public:
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 tex_coord;

		bool operator==(const Vertex& other) const {
			return pos == other.pos && color == other.color && tex_coord == other.tex_coord;
		}
	};

	class MeshData
	{
	public:
		std::vector<Vertex> vertex_buffer;
		std::vector<int> index_buffer;
	};
}