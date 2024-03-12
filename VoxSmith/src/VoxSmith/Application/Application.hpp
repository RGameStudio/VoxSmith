#pragma once

#include <memory>

#include "../Core.hpp"
#include "../Window/Window.hpp"

namespace VoxSmith
{
	class VOX_SMITH_API Application
	{
	public:
		Application();
		virtual ~Application();

		void run();

		Application(const Application&) = delete;
		Application(Application&&) = delete;
		Application& operator=(const Application&) = delete;
		Application& operator=(Application&&) = delete;

	private:
		std::unique_ptr<Window> m_window;
	};

	Application* createApplication();
}
