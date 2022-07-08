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
		float nx, ny, nz;
		float u, v;

		bool operator==(const GVertex& other) const {
			return x == other.x && y == other.y && z == other.z && nx == other.nx && ny == other.ny && nz == other.nz && u == other.u && v == other.v;
		}
	};

	class Transform
	{
	public:
		glm::mat4 transform;
		glm::mat3 n_transform;
	};

	class Vertex
	{
	public:
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 tex_coord;

		bool operator==(const Vertex& other) const {
			return pos == other.pos && normal == other.normal && tex_coord == other.tex_coord;
		}
	};

	class MeshData
	{
	public:
		struct AABB {
			glm::vec3 minB, maxB; // min and max bound for XYZ
			glm::vec3 center;
			glm::vec3 points[8];
		};

		void setAABB(glm::vec3 minB, glm::vec3 maxB) {
			aabb = { minB, maxB, glm::vec3(0.5f, 0.5f, 0.5f) * (minB + maxB) };
			aabb.points[0] = aabb.minB;
			aabb.points[1] = glm::vec3(aabb.minB[0], aabb.minB[1], aabb.maxB[2]);
			aabb.points[2] = glm::vec3(aabb.minB[0], aabb.maxB[1], aabb.minB[2]);
			aabb.points[3] = glm::vec3(aabb.minB[0], aabb.maxB[1], aabb.maxB[2]);
			aabb.points[4] = glm::vec3(aabb.maxB[0], aabb.minB[1], aabb.minB[2]);
			aabb.points[5] = glm::vec3(aabb.maxB[0], aabb.minB[1], aabb.maxB[2]);
			aabb.points[6] = glm::vec3(aabb.maxB[0], aabb.maxB[1], aabb.minB[2]);
			aabb.points[7] = aabb.maxB;
		}

		std::vector<Vertex> vertex_buffer;
		std::vector<int> index_buffer;
		AABB aabb;
	};
}