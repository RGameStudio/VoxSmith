#include <functional>

#include "../Event/Event.hpp"
#include "../Logger/Log.hpp"
#include "../Shader/Shader.hpp"
#include "../Input/Input.hpp"

#include "Application.hpp"

using namespace VoxSmith;

Application::Application(const uint32_t width, const uint32_t height)
	: m_isRunning(true)
{
	Log::init();

	m_window = createWindow(width, height, "VoxSmithDemo");
	m_window->setWindowCallback(std::bind(&Application::handleEvents, this, std::placeholders::_1));
	m_UICanvas = std::make_unique<UICanvasIMGUI>(m_window);
}

Application::~Application() noexcept
{
}

void Application::update(const float dt)
{

}

void Application::updateCamera(const float dt)
{
		
}

void Application::draw(const float dt, const float cframe)
{

}

void Application::run()
{
	float previousFrame = 0.0f;

	while (m_isRunning)
	{
		float currentFrame = m_window->getTime();
		float dt = currentFrame - previousFrame;
		previousFrame = currentFrame;

		m_window->clearBuffers();

		update(dt);
		draw(dt, currentFrame);

		m_UICanvas->update();
		m_window->swapBuffers();
	}
}

void Application::handleEvents(WindowEvent& e)
{
	switch (e.getType())
	{
	case WindowEventType::MOUSE_MOVE: {
		break;
	}

	case WindowEventType::MOUSE_CLICK:
		break;

	case WindowEventType::KEYBOARD: {
		KeyboardEvent event = static_cast<KeyboardEvent&>(e);
		Keyboard::getInstance().setKeyStatus(event.m_key, static_cast<KeyStatus>(event.m_action));

		break;
	}

	case WindowEventType::WINDOW_RESIZE: {
		ResizeEvent event = static_cast<ResizeEvent&>(e);
		break;
	}

	case WindowEventType::CLOSE:
		m_isRunning = false;
		break;
	}
}
