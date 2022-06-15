#pragma once

#include <vector>
#include <string>

namespace Eagle
{
	class Vertex
	{
	public:
		float x, y, z;
		float r, g, b;
		float u, v;
	};

	class MeshData
	{
	public:
		std::vector<Vertex> vertex_buffer;
		std::vector<int> index_buffer;
	};
}