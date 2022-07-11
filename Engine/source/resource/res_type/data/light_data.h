#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <vector>
#include <string>

namespace Eagle
{
	class DirectionalLight
	{
	public:
		glm::vec3 direction;
		glm::vec3 intensity;
		float ambient;
	};
}