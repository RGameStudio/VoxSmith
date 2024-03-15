#include <functional>
#include <iostream>

#include "../Logger/Log.hpp"
#include "../Event/Event.hpp"

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
		m_window->update();
	}
}

void Application::handleEvents(WindowEvent& e)
{
	switch (e.getType())
	{
	case WindowEventType::MouseMove: {
		MouseMoveEvent event = static_cast<MouseMoveEvent&>(e);
		LOG_CORE_INFO("MouseMoveEvent x:{} y:{}", event.x, event.y);
	}break;

	case WindowEventType::MouseClick:
		LOG_CORE_INFO("MouseClickEvent");
		break;

	case WindowEventType::Keyboard: {
		KeyboardEvent event = static_cast<KeyboardEvent&>(e);
		LOG_CORE_INFO("KeyboardEvent: {}", (char)event.m_key);
	}break;

	case WindowEventType::Close:
		LOG_CORE_INFO("CloseEvent");
		m_isRunning = false;
		break;
	}
}
