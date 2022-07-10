#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <vector>

namespace Eagle
{
	struct CameraData
	{
		glm::vec3 m_position;
		glm::vec2 m_aspect;
		float m_yaw;
		float m_pitch;
		float m_z_near;
		float m_z_far;
	};

	const glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	class Camera
	{
	public:
		Camera(CameraData init_data) {
			m_data = init_data;
			update();
		}
		~Camera() {};

		glm::mat4 getViewProj() {
			auto view = glm::lookAt(m_data.m_position, m_data.m_position + m_front, m_up);
			auto proj = glm::perspective(m_data.m_aspect.x, m_data.m_aspect.y, m_data.m_z_near, m_data.m_z_far);
			proj[1][1] *= -1;
			return proj * view;
		}
		glm::mat4 getView() {
			return glm::lookAt(m_data.m_position, m_data.m_position + m_front, m_up);
		}
		glm::mat4 getProj() {
			auto proj = glm::perspective(m_data.m_aspect.x, m_data.m_aspect.y, m_data.m_z_near, m_data.m_z_far);
			proj[1][1] *= -1;
			return proj;
		}

		CameraData m_data;
		glm::vec3 m_front;
		glm::vec3 m_up;
		glm::vec3 m_right;
		float m_speed = 1.0f;
		float m_sensitivity = 1.0f;

		void move(glm::vec3 forward) {
			m_data.m_position += forward * m_speed;
		}

		void rotate(float xoffset, float yoffset, bool constrainPitch = true) {
			xoffset *= m_sensitivity;
			yoffset *= m_sensitivity;

			m_data.m_yaw += xoffset;
			m_data.m_pitch += yoffset;

			// make sure that when pitch is out of bounds, screen doesn't get flipped
			if (constrainPitch)
			{
				if (m_data.m_pitch > 89.0f)
					m_data.m_pitch = 89.0f;
				if (m_data.m_pitch < -89.0f)
					m_data.m_pitch = -89.0f;
			}
		}

		void update() {
			glm::vec3 front;
			front.x = cos(glm::radians(m_data.m_yaw)) * cos(glm::radians(m_data.m_pitch));
			front.y = sin(glm::radians(m_data.m_pitch));
			front.z = sin(glm::radians(m_data.m_yaw)) * cos(glm::radians(m_data.m_pitch));
			m_front = glm::normalize(front);
			m_right = glm::normalize(glm::cross(m_front, WorldUp));
			m_up = glm::normalize(glm::cross(m_right, m_front));
		}
	};
}