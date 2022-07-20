#include "function/input/input_system.h"

namespace Eagle
{
	void InputSystem::initialize(InputSystemInitInfo init_info)
	{
		m_window_system = init_info.window_system;
		m_window_system->registerOnKeyFunc(std::bind(&InputSystem::onKey,
			this,
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3,
			std::placeholders::_4));
		m_window_system->registerOnCursorPosFunc(
			std::bind(&InputSystem::onCursorPos, this, std::placeholders::_1, std::placeholders::_2));
		m_window_system->registerOnMouseButtonFunc(std::bind(&InputSystem::onMouseButton, this,
			std::placeholders::_1, 
			std::placeholders::_2,
			std::placeholders::_3));
		m_render_system = init_info.render_system;
		m_world = init_info.world;

		m_keys[GLFW_KEY_W] = false;
		m_keys[GLFW_KEY_S] = false;
		m_keys[GLFW_KEY_A] = false;
		m_keys[GLFW_KEY_D] = false;
		m_keys[GLFW_KEY_Q] = false;
		m_keys[GLFW_KEY_E] = false;
	}

	void InputSystem::tick(float delta_time)
	{
		if (m_mouse_left) {
			float angularVelocity = 180.0f / glm::max(m_window_system->getWindowSize()[0], m_window_system->getWindowSize()[1]);
			Camera& camera = m_world->m_current_scene->getCamera();
			camera.rotate(m_cursor_delta_x * angularVelocity, -m_cursor_delta_y * angularVelocity);
			camera.update();
			m_cursor_delta_x = 0.0f;
			m_cursor_delta_y = 0.0f;
		}
		if (m_keys[GLFW_KEY_W]) {
			Camera& camera = m_world->m_current_scene->getCamera();
			glm::vec3 forward = delta_time * camera.m_front;
			camera.move(forward);
		}
		if (m_keys[GLFW_KEY_S]) {
			Camera& camera = m_world->m_current_scene->getCamera();
			glm::vec3 forward = -delta_time * camera.m_front;
			camera.move(forward);
		}
		if (m_keys[GLFW_KEY_A]) {
			Camera& camera = m_world->m_current_scene->getCamera();
			glm::vec3 forward = -delta_time * camera.m_right;
			camera.move(forward);
		}
		if (m_keys[GLFW_KEY_D]) {
			Camera& camera = m_world->m_current_scene->getCamera();
			glm::vec3 forward = delta_time * camera.m_right;
			camera.move(forward);
		}
		if (m_keys[GLFW_KEY_E]) {
			Camera& camera = m_world->m_current_scene->getCamera();
			glm::vec3 forward = delta_time * WorldUp;
			camera.move(forward);
		}
		if (m_keys[GLFW_KEY_Q]) {
			Camera& camera = m_world->m_current_scene->getCamera();
			glm::vec3 forward = -delta_time * WorldUp;
			camera.move(forward);
		}
		if (m_keys[GLFW_KEY_R]) {
			m_render_system->m_reload = true;
			m_keys[GLFW_KEY_R] = false;
		}
	}

	void InputSystem::cleanup()
	{
		m_keys.clear();
	}

	void InputSystem::onKey(int key, int scancode, int action, int mods)
	{
		if (action == GLFW_PRESS)
		{
			m_keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			m_keys[key] = false;
		}
	}

	void InputSystem::onCursorPos(double current_cursor_x, double current_cursor_y)
	{
		m_cursor_delta_x = current_cursor_x - m_cursor_last_x;
		m_cursor_delta_y = current_cursor_y - m_cursor_last_y;
		m_cursor_last_x = current_cursor_x;
		m_cursor_last_y = current_cursor_y;
	}

	void InputSystem::onMouseButton(int button, int action, int mods)
	{
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:
			m_mouse_left = action == GLFW_PRESS; break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			m_mouse_right = action == GLFW_PRESS; break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			m_mouse_middle = action == GLFW_PRESS; break;
		default:
			break;
		}	
	}
}