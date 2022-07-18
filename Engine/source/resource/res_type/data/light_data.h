#pragma once

#include "camera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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

		// Inputs: current camera and shadow distance
		glm::mat4 getViewProj(const Camera& camera, float distance) {
			auto view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), direction, abs(direction[1]) > 0.999f ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f));

			float fov = camera.m_data.m_aspect.x, aspect = camera.m_data.m_aspect.y;
			glm::vec3 p = camera.m_data.m_position + distance * camera.m_front;
			float w2 = distance * tanf(fov / 2.0f);
			glm::vec3 r = w2 * camera.m_right;
			glm::vec3 u = w2 * camera.m_up / aspect;
			glm::vec3 points[4] = {
				p + r + u,
				p + r - u,
				p - r + u,
				p - r - u
			};

			glm::vec3 minB = camera.m_data.m_position, maxB = camera.m_data.m_position; // min and max bound for XYZ

			for (int i = 0; i < 4; i++) {
				minB = glm::min(minB, points[i]);
				maxB = glm::max(maxB, points[i]);
			}		

			auto proj = glm::ortho(minB.x, maxB.x, minB.y, maxB.y, minB.z, maxB.z);
			//proj[1][1] *= -1;
			return proj * view;
		}

	};
}