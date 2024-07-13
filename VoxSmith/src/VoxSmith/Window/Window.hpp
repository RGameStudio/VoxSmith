#pragma once

#include <functional>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "VoxSmith/Core.hpp"

namespace VoxSmith
{
	struct WindowEvent;
	using WindowEventCallbackFn = std::function<void(WindowEvent& e)>;
	
	class VOX_SMITH_API Window final
	{
	public:
		friend std::shared_ptr<Window> createWindow(const size_t width, const size_t height, const char* title = "");

		~Window() noexcept;
		
		void setWindowCallback(const WindowEventCallbackFn& fn);
		void clearBuffers();
		void swapBuffers();

		inline GLFWwindow* getInstance() const { return m_window; }
		inline float getTime() const { return glfwGetTime(); }

		inline size_t getWidth() const { return m_width; }
		inline size_t getHeight() const { return m_height; }

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
