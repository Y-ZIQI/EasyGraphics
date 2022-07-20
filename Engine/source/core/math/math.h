#pragma once

#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE 1
#endif

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

namespace Eagle
{
	class Math {
	public:
		static glm::mat4 ortho(
			float left, float right,
			float bottom, float top,
			float zNear, float zFar
		);
	};
}
