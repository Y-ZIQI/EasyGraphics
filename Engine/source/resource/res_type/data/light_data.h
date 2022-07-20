#pragma once

#include "core/math/math.h"

#include "camera.h"

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
		float distance_to_camera = 10.0f;

		// Inputs: current camera and shadow distance
		glm::mat4 getViewProj(const Camera& camera, float distance) {
			const glm::vec3& cp = camera.m_data.m_position;

			auto view = glm::lookAt(cp - distance_to_camera * direction, cp, abs(direction[1]) > 0.999f ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f));

			float fov = camera.m_data.m_aspect.x, aspect = camera.m_data.m_aspect.y;
			glm::vec3 p = cp + distance * camera.m_front;
			float w2 = distance * tanf(fov / 2.0f);
			glm::vec3 r = w2 * camera.m_right;
			glm::vec3 u = w2 * camera.m_up / aspect;
			glm::vec3 points[4] = {
				p + r + u,
				p + r - u,
				p - r + u,
				p - r - u
			};

			glm::vec4 p0 = view * glm::vec4(cp, 1.0);
			p0 = p0 / p0.w;
			glm::vec4 minB = p0, maxB = p0; // min and max bound for XYZ

			for (int i = 0; i < 4; i++) {
				glm::vec4 np = view * glm::vec4(points[i], 1.0);
				np = np / np.w;
				minB = glm::min(minB, np);
				maxB = glm::max(maxB, np);
			}		

			auto proj = Math::ortho(minB.x - 1.0f, maxB.x + 1.0f, -maxB.y - 1.0f, -minB.y + 1.0f, -maxB.z - distance_to_camera, -minB.z + distance_to_camera);
			proj[1][1] *= -1;
			return proj * view;
		}

	};
}