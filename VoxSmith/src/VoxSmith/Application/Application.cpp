#include <functional>
#include <iostream>

#include "../Logger/Log.hpp"
#include "../Event/Event.hpp"

#include "Application.hpp"

using namespace VoxSmith;

Application::Application()
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
	while (m_window->canClose())
	{
		m_window->update();
	}
}

void Application::handleEvents(WindowEvent& e)
{
	switch (e.getType())
	{
	case WindowEventType::MouseMove:
		LOG_CORE_INFO("MouseMoveEvent");
		break;

	case WindowEventType::MouseClick:
		LOG_CORE_INFO("MouseClickEvent");
		break;

	case WindowEventType::Keyboard: {
		LOG_CORE_INFO("KeyboardEvent");
	}break;

	case WindowEventType::Close:
		LOG_CORE_INFO("CloseEvent");
		break;
	}
}
