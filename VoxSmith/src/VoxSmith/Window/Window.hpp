#pragma once

#include <GLFW/glfw3.h>

#include "VoxSmith/Core.hpp"

namespace VoxSmith
{
	class VOX_SMITH_API Window final
	{
	public:
		Window(size_t width, size_t height);
		~Window();

		Window() = delete;
		Window(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(const Window&) = delete;
		Window& operator=(Window&&) = delete;

	private:
		size_t m_width;
		size_t m_height;

		GLFWwindow* m_window;
	};
}
