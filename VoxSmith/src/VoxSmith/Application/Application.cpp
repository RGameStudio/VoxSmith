#include <functional>

#include "../Event/Event.hpp"
#include "../Logger/Log.hpp"
#include "../UICanvasIMGUI/UICanvasIMGUI.hpp"
#include "../Shader/Shader.hpp"

#include "Application.hpp"

using namespace VoxSmith;

constexpr size_t g_tempWidth = 800.0f;
constexpr size_t g_tempHeight = 800.0f;

Application::Application()
	: m_isRunning(true)
{
	Log::init();

	m_window = Window::create(g_tempWidth, g_tempHeight, "VoxSmithDemo");
	m_window->setWindowCallback(std::bind(&Application::handleEvents, this, std::placeholders::_1));

	m_UICanvas = new UICanvasIMGUI(m_window->getInstance());
}

Application::~Application() noexcept
{
	delete m_UICanvas;
}

void Application::update(float dt)
{

}

void Application::draw()
{

}

void Application::run()
{
	float currentFrame = m_window->getTime();
	float previousFrame = 0.0f;

	while (m_isRunning)
	{
		float dt = currentFrame - previousFrame;
		previousFrame = currentFrame;

		m_window->clearBuffers();
		m_UICanvas->update();

		update(dt);
		draw();

		m_window->swapBuffers();
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
		LOG_CORE_TRACE("CloseEvent");
		m_isRunning = false;
		break;
	}
}
