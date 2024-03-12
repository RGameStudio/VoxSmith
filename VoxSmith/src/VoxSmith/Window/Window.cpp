#include "../Logger/Log.hpp"

#include "Window.hpp"

using namespace VoxSmith;

Window::Window(size_t width, size_t height)
	: m_width(width)
	, m_height(height)
{
	if (!glfwInit())
	{
		LOG_CORE_ERROR("Could not initialize GLFW");
	}

	m_window = glfwCreateWindow(width, height, "VoxSmithDemo", nullptr, nullptr);
	glfwMakeContextCurrent(m_window);
}

Window::~Window()
{
	glfwDestroyWindow(m_window);
}
