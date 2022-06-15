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

	bool WindowSystem::windowShouldClose()
	{
		return glfwWindowShouldClose(m_window);
	}

	void WindowSystem::pollEvents()
	{
		glfwPollEvents();
	}
}