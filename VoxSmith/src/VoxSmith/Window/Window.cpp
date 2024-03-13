#include <memory>

#include "../Logger/Log.hpp"

#include "Window.hpp"

using namespace VoxSmith;

std::unique_ptr<Window> Window::create(const size_t width, const size_t height, const char* title)
{
	static bool created = false;
	if (created)
	{
		return nullptr;
	}

	return std::unique_ptr<Window>(new Window(width, height, title));
}

Window::Window(const size_t width, const size_t height, const char* title)
	: m_width(width)
	, m_height(height)
{
	if (!glfwInit())
	{
		LOG_CORE_ERROR("Could not initialize GLFW");
	}

	m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	glfwMakeContextCurrent(m_window);
}

Window::~Window()
{
	glfwDestroyWindow(m_window);
}

void Window::endFrame()
{
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}
