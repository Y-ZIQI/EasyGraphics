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
		glm::mat4 getViewProj(const Camera& camera, float near, float far) {
			const float bias = 0.1f;
			const glm::vec3& cp = camera.m_data.m_position;

			auto view = glm::lookAt(cp - distance_to_camera * direction, cp, abs(direction[1]) > 0.999f ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f));

			float fov = camera.m_data.m_aspect.x, aspect = camera.m_data.m_aspect.y;
			glm::vec3 np = cp + near * camera.m_front, fp = cp + far * camera.m_front;
			float nh = near * tanf(fov / 2.0f), fh = far * tanf(fov / 2.0f);
			glm::vec3 nr = nh * camera.m_right * aspect, fr = fh * camera.m_right * aspect;
			glm::vec3 nu = nh * camera.m_up, fu = fh * camera.m_up;
			glm::vec3 points[8] = {
				np + nr + nu,
				np + nr - nu,
				np - nr + nu,
				np - nr - nu,
				fp + fr + fu,
				fp + fr - fu,
				fp - fr + fu,
				fp - fr - fu
			};

			glm::vec4 p0 = view * glm::vec4(points[0], 1.0);
			p0 = p0 / p0.w;
			glm::vec4 minB = p0, maxB = p0; // min and max bound for XYZ
			for (int i = 1; i < 8; i++) {
				glm::vec4 pt = view * glm::vec4(points[i], 1.0);
				pt = pt / pt.w;
				minB = glm::min(minB, pt);
				maxB = glm::max(maxB, pt);
			}

			auto proj = Math::ortho(minB.x, maxB.x, -maxB.y, -minB.y, -maxB.z - distance_to_camera, -minB.z);
			proj[1][1] *= -1;
			return proj * view;
		}

	};
}