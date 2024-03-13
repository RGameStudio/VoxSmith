#pragma once

#include <memory>

#include "../Core.hpp"
#include "../Window/Window.hpp"

namespace VoxSmith
{
	struct WindowEvent;

	class VOX_SMITH_API Application
	{
	public:
		Application();
		virtual ~Application();

		virtual void run();
		virtual void handleEvents(WindowEvent& e);

		Application(const Application&) = delete;
		Application(Application&&) = delete;
		Application& operator=(const Application&) = delete;
		Application& operator=(Application&&) = delete;

	private:
		std::unique_ptr<Window> m_window = nullptr;
	};

	Application* createApplication();
}
