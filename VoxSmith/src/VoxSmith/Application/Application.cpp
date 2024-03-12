#include <iostream>

#include "../Logger/Log.hpp"

#include "Application.hpp"

using namespace VoxSmith;

Application::Application()
{
	Log::init();

	m_window = std::unique_ptr<Window>(new Window(500.0f, 500.0f));
}

Application::~Application()
{

}

void Application::run()
{
	while (true)
	{

	}
}
