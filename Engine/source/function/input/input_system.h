#pragma once

#include "function/render/window_system.h"

#include <set>

namespace Eagle
{
	struct InputSystemInitInfo
	{
		std::shared_ptr<WindowSystem> window_system;
	};

	class InputSystem
	{
	public:
		InputSystem() {};
		~InputSystem() {};

		void initialize(InputSystemInitInfo init_info);
		void tick();
		void cleanup();

		void onKey(int key, int scancode, int action, int mods);
		void onCursorPos(double current_cursor_x, double current_cursor_y);
		void onMouseButton(int button, int action, int mods);

		std::set<int> m_keys;
		
		bool m_mouse_left;
		bool m_mouse_right;
		bool m_mouse_middle;

		float m_cursor_last_x;
		float m_cursor_last_y;
		float m_cursor_delta_x;
		float m_cursor_delta_y;

		std::shared_ptr<WindowSystem> m_window_system;
	};
}