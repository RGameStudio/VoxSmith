#include <functional>

#include "../Event/Event.hpp"
#include "../Logger/Log.hpp"

#include "Application.hpp"

using namespace VoxSmith;

Application::Application()
	: m_isRunning(true)
{
	Log::init();

	m_window = Window::create(800.0f, 800.0f, "VoxSmithDemo");
	m_window->setWindowCallback(std::bind(&Application::handleEvents, this, std::placeholders::_1));
}

Application::~Application()
{

} 

void Application::run()
{
	while (m_isRunning)
	{
		glClearColor(1.0f, 0.5f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		m_window->update();
	}
}

void Application::handleEvents(WindowEvent& e)
{
	switch (e.getType())
	{
	case WindowEventType::MOUSE_MOVE: {
		MouseMoveEvent event = static_cast<MouseMoveEvent&>(e);
		LOG_CORE_TRACE("MouseMoveEvent x:{} y:{}", event.x, event.y);
		break;
	}

	case WindowEventType::MOUSE_CLICK:
		LOG_CORE_TRACE("MouseClickEvent");
		break;

	case WindowEventType::KEYBOARD: {
		KeyboardEvent event = static_cast<KeyboardEvent&>(e);
		LOG_CORE_TRACE("KeyboardEvent: {}", static_cast<char>(event.m_key));
		break;
	}

	case WindowEventType::WINDOW_RESIZE: {
		ResizeEvent event = static_cast<ResizeEvent&>(e);
		LOG_CORE_TRACE("ResizeEvent: width:{} height:{}", event.m_width, event.m_height);
		break;
	}

	case WindowEventType::CLOSE:
		LOG_CORE_INFO("CloseEvent");
		m_isRunning = false;
		break;
	}
}
