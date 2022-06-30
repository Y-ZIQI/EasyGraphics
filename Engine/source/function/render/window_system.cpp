#include "function/render/window_system.h"

namespace Eagle
{
	void WindowSystem::initialize(WindowCreateInfo create_info)
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		m_width = create_info.width;
		m_height = create_info.height;
		m_title = create_info.title;

		m_window = glfwCreateWindow(m_width, m_height, create_info.title, nullptr, nullptr);
		glfwSetWindowUserPointer(m_window, this);
		//glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
		glfwSetKeyCallback(m_window, keyCallback);
		glfwSetCharCallback(m_window, charCallback);
		glfwSetCharModsCallback(m_window, charModsCallback);
		glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
		glfwSetCursorPosCallback(m_window, cursorPosCallback);
		glfwSetCursorEnterCallback(m_window, cursorEnterCallback);
		glfwSetScrollCallback(m_window, scrollCallback);
		glfwSetDropCallback(m_window, dropCallback);
		glfwSetWindowSizeCallback(m_window, windowSizeCallback);
		glfwSetWindowCloseCallback(m_window, windowCloseCallback);

		//glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
	}

	void WindowSystem::cleanup()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	GLFWwindow* WindowSystem::getWindow() const
	{
		return m_window;
	}

	std::array<int, 2> WindowSystem::getWindowSize() const
	{
		return std::array<int, 2>({ m_width, m_height });
	}

	void WindowSystem::setFocusMode(bool mode)
	{
		m_is_focus_mode = mode;
		glfwSetInputMode(m_window, GLFW_CURSOR, m_is_focus_mode ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}

	bool WindowSystem::windowShouldClose()
	{
		return glfwWindowShouldClose(m_window);
	}

	void WindowSystem::pollEvents()
	{
		glfwPollEvents();
	}
}