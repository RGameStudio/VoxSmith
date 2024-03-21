#pragma once

#include <memory>

#include "../Core.hpp"
#include "../Window/Window.hpp"

namespace VoxSmith
{
	struct WindowEvent;
	class UICanvasIMGUI;

	class VOX_SMITH_API Application
	{
	public:
		Application();
		virtual ~Application();

		// this function is derived in Sandbox where the game is supposed to happen
		virtual void update();

		void run();
		void handleEvents(WindowEvent& e);

		Application(const Application&) = delete;
		Application(Application&&) = delete;
		Application& operator=(const Application&) = delete;
		Application& operator=(Application&&) = delete;

	private:
		std::unique_ptr<Window> m_window = nullptr;
		UICanvasIMGUI* m_UICanvas = nullptr;

		bool m_isRunning = false;
	};

	Application* createApplication();
}
