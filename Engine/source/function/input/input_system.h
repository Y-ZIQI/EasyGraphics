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

		std::set<int> m_keys;

		int m_cursor_last_x;
		int m_cursor_last_y;
		int m_cursor_delta_x;
		int m_cursor_delta_y;

		std::shared_ptr<WindowSystem> m_window_system;
	};
}