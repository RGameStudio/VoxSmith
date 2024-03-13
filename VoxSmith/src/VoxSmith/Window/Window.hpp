#pragma once

#include <GLFW/glfw3.h>

#include "VoxSmith/Core.hpp"

namespace VoxSmith
{
	struct WindowCallbackData
	{

	};

	class VOX_SMITH_API Window final
	{
	public:
		static std::unique_ptr<Window> create(const size_t width, const size_t height, const char* title);

		~Window();

		inline bool canClose() { return !glfwWindowShouldClose(m_window); }

		void endFrame();

		Window() = delete;
		Window(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(const Window&) = delete;
		Window& operator=(Window&&) = delete;

	private:
		Window(const size_t width, const size_t height, const char* title);

		size_t m_width;
		size_t m_height;

		GLFWwindow* m_window;
	};
}
