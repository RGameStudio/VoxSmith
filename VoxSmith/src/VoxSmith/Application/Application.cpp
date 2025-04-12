#include <functional>

#include "VoxSmith/Event/Event.hpp"
#include "VoxSmith/Logger/Log.hpp"
#include "VoxSmith/Shader/Shader.hpp"
#include "VoxSmith/Input/Input.hpp"
#include "VoxSmith/Timer/Timestep.hpp"
#include "VoxSmith/ResourceManager/ResourceManager.hpp"

#include "Application.hpp"

using namespace VoxSmith;

Application::Application(const uint32_t width, const uint32_t height)
	: m_isRunning(true)
{
	Log::init();

	//std::bind(addOne, )
	m_window = createWindow(width, height, "VoxSmithDemo");
	m_window->setWindowCallback(std::bind(&Application::handleEvents, this, std::placeholders::_1));
	m_renderer = std::make_shared<Renderer>();
	m_UICanvas = std::make_unique<UICanvasIMGUI>(m_window);
	m_camera = std::make_unique<Camera>(glm::vec3(0.0f, 300.0f, 5.0f), width, height);
	
	ResourceManager::getInstance().initShaders();
	ResourceManager::getInstance().initTextureList();
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
	while (m_isRunning)
	{
		float currentFrame = m_window->getTime();
		float dt = currentFrame - m_lastFrame;
		m_lastFrame = currentFrame;

		Timestep tStep = dt;

		m_window->clearBuffers();
		
		{
			update(dt);
			m_UICanvas->iniFrame();
			m_UICanvas->setCameraInfo(m_camera);
			m_UICanvas->setChunkInfo(m_renderer->m_cullingStatus);
		}

		{
			m_renderer->switchCulling();
			m_renderer->m_showEdges = m_UICanvas->getEdgesRenderStatus();

			draw(dt, currentFrame);
			m_UICanvas->render();
		}
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

	case WindowEventType::CLOSE: {
		m_isRunning = false;
		break;
	}
	}
}
