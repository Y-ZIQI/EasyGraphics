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
	}

	void InputSystem::tick()
	{
	}

	void InputSystem::cleanup()
	{
	}

	void InputSystem::onKey(int key, int scancode, int action, int mods)
	{
		if (action == GLFW_PRESS)
		{
			m_keys.insert(key);
		}
		else if (action == GLFW_RELEASE)
		{
			m_keys.erase(key);
		}
	}

	void InputSystem::onCursorPos(double current_cursor_x, double current_cursor_y)
	{
		m_cursor_delta_x = m_cursor_last_x - current_cursor_x;
		m_cursor_delta_y = m_cursor_last_y - current_cursor_y;
		m_cursor_last_x = current_cursor_x;
		m_cursor_last_y = current_cursor_y;
	}
}