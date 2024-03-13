#include <iostream>

#include "../Logger/Log.hpp"

#include "Application.hpp"

using namespace VoxSmith;

Application::Application()
{
	Log::init();

	m_window = Window::create(800.0f, 800.0f, "VoxSmithDemo");
}

Application::~Application()
{

}

void Application::run()
{
	while (m_window->canClose())
	{
		m_window->endFrame();
	}
}
