#pragma once

#include <functional>

#include <GLFW/glfw3.h>

#include "VoxSmith/Core.hpp"

namespace VoxSmith
{
	struct WindowEvent;
	using WindowEventCallbackFn = std::function<void(WindowEvent& e)>;
	
	class VOX_SMITH_API Window final
	{
	public:
		static std::unique_ptr<Window> create(const size_t width, const size_t height, const char* title = "");

		~Window() noexcept;
		
		void setWindowCallback(const WindowEventCallbackFn& fn);
		void update();

		Window() = delete;
		Window(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(const Window&) = delete;
		Window& operator=(Window&&) = delete;

	private:
		Window(const size_t width, const size_t height, const char* title = "");

		size_t m_width;
		size_t m_height;

		GLFWwindow* m_window = nullptr;

		struct WindowCallbackWrapper 
		{
			WindowEventCallbackFn fn;
		} m_wrapper;
	};
}
