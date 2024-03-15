#include <memory>

#include "../Logger/Log.hpp"
#include "../Event/Event.hpp"

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
		LOG_CORE_ERROR("Could not initialize GLFW.");
	}

	m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	LOG_CORE_INFO("GLFW window was cretaed.");
	glfwMakeContextCurrent(m_window);
	glfwSetWindowUserPointer(m_window, &m_wrapper);

	glfwSetKeyCallback(m_window,
		[](GLFWwindow* window, int key, int scancode, int action, int mods) {
			const auto* const wrapper = 
				static_cast<const WindowCallbackWrapper* const>(glfwGetWindowUserPointer(window));
			
			KeyboardEvent e;
			e.m_action = action;
			e.m_key = key;
			wrapper->fn(e);
		});

	glfwSetCursorPosCallback(m_window,
		[](GLFWwindow* window, double xPos, double yPos) {
			const auto* const wrapper = 
				static_cast<const WindowCallbackWrapper* const>(glfwGetWindowUserPointer(window));

			MouseMoveEvent e;
			e.x = xPos;
			e.y = yPos;
			wrapper->fn(e);
		});

	glfwSetMouseButtonCallback(m_window,
		[](GLFWwindow* window, int button, int action, int mods) {
			const auto* const wrapper = 
				static_cast<const WindowCallbackWrapper* const>(glfwGetWindowUserPointer(window));

			MouseClickEvent e;
			e.m_button = button;
			e.m_action = action;
			wrapper->fn(e);
		});


	glfwSetWindowCloseCallback(m_window,
		[](GLFWwindow* window) {
			auto* const wrapper = 
				static_cast<const WindowCallbackWrapper* const>(glfwGetWindowUserPointer(window));

			wrapper->fn(CloseEvent());
		});
	
}

Window::~Window() noexcept
{
	glfwDestroyWindow(m_window);
}

void Window::update()
{
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void Window::setWindowCallback(const WindowEventCallbackFn& fn)
{
	m_wrapper.fn = fn;
}
