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
	m_renderer = std::make_shared<Renderer>();
	m_UICanvas = std::make_unique<UICanvasIMGUI>(m_window);
	m_camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 5.0f), width, height);
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
		m_UICanvas->update();
		m_UICanvas->setCameraInfo(m_camera->getPos());
		m_UICanvas->setChunkInfo(m_renderer->m_cullingStatus);

		m_renderer->m_showEdges = m_UICanvas->getEdgesRenderStatus();

		draw(dt, currentFrame);
		m_UICanvas->render();

		m_window->swapBuffers();
	}
}

void Application::handleEvents(WindowEvent& e)
{
	switch (e.getType())
	{
	case WindowEventType::MOUSE_MOVE: {
		auto event = static_cast<MouseMoveEvent&>(e);
		Mouse::getInstance().setMousePos(event.x, event.y);
		break;
	}

	case WindowEventType::MOUSE_CLICK:
		break;

	case WindowEventType::KEYBOARD: {
		auto event = static_cast<KeyboardEvent&>(e);
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
