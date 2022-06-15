#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <array>
#include <functional>
#include <vector>
#include <string>

namespace Eagle
{
	struct WindowCreateInfo
	{
		int         width{ 1280 };
		int         height{ 720 };
		const char* title{ "Eagle" };
		bool        is_fullscreen{ false };
	};

	class WindowSystem 
	{
	public:
		WindowSystem() {};
		~WindowSystem() {};

		void initialize(WindowCreateInfo create_info);
		void cleanup();

		GLFWwindow* getWindow() const;
		bool windowShouldClose();
		void pollEvents();

	private:
		GLFWwindow* m_window{ nullptr };
		int         m_width{ 0 };
		int         m_height{ 0 };
		std::string m_title;

		bool m_is_focus_mode{ false };
	};
}