#pragma once

#include "function/render/window_system.h"
#include "function/world/world_manager.h"

#include <set>
#include <unordered_map>

namespace Eagle
{
	struct InputSystemInitInfo
	{
		std::shared_ptr<WindowSystem> window_system;
		std::shared_ptr<WorldManager> world;
	};

	class InputSystem
	{
	public:
		InputSystem() {};
		~InputSystem() {};

		void initialize(InputSystemInitInfo init_info);
		void tick(float delta_time);
		void cleanup();

		void onKey(int key, int scancode, int action, int mods);
		void onCursorPos(double current_cursor_x, double current_cursor_y);
		void onMouseButton(int button, int action, int mods);

		//std::set<int> m_keys;
		std::unordered_map<int, bool> m_keys;
		
		bool m_mouse_left = false;
		bool m_mouse_right = false;
		bool m_mouse_middle = false;

		float m_cursor_last_x = 0.0f;
		float m_cursor_last_y = 0.0f;
		float m_cursor_delta_x = 0.0f;
		float m_cursor_delta_y = 0.0f;

		std::shared_ptr<WindowSystem> m_window_system;
		std::shared_ptr<WorldManager> m_world;
	};
}