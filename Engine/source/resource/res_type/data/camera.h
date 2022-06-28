#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <vector>

namespace Eagle
{
	class CameraData
	{
	public:
		glm::vec3 m_position;
		glm::vec3 m_target;
		glm::vec3 m_up;
		glm::vec2 m_aspect;
		float m_z_near;
		float m_z_far;
	};

	class Camera
	{
	public:
		Camera(CameraData init_data) {
			m_data = init_data;
		}
		~Camera() {};

		glm::mat4 getViewProj() {
			auto view = glm::lookAt(m_data.m_position, m_data.m_target, m_data.m_up);
			auto proj = glm::perspective(m_data.m_aspect.x, m_data.m_aspect.y, m_data.m_z_near, m_data.m_z_far);
			proj[1][1] *= -1;
			return proj * view;
		}
		glm::mat4 getView() {
			return glm::lookAt(m_data.m_position, m_data.m_target, m_data.m_up);
		}
		glm::mat4 getProj() {
			auto proj = glm::perspective(m_data.m_aspect.x, m_data.m_aspect.y, m_data.m_z_near, m_data.m_z_far);
			proj[1][1] *= -1;
			return proj;
		}

		CameraData m_data;
	};
}