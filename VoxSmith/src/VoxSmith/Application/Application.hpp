#pragma once

#include <queue>
#include <memory>

#include "../Core.hpp"
#include "../Window/Window.hpp"
#include "../UICanvasIMGUI/UICanvasIMGUI.hpp"
#include "../Renderer/Renderer.hpp"

namespace VoxSmith
{
	struct WindowEvent;

	class VOX_SMITH_API Application
	{
	public:
		Application(const uint32_t width, const uint32_t height);
		virtual ~Application() noexcept;

		// @WORK_FOR_NOW: these functions are derived in Sandbox where the client code is allowed
		virtual void update(float dt);
		virtual void draw(float dt, float cframe);

		void run();
		void handleEvents(WindowEvent& e);
		void handleClientInput();

		Application() = delete;
		Application(const Application&) = delete;
		Application(Application&&) = delete;
		Application& operator=(const Application&) = delete;
		Application& operator=(Application&&) = delete;

	private:
		std::shared_ptr<Window> m_window = nullptr;
		std::shared_ptr<Renderer> m_renderer = nullptr;
		std::unique_ptr<UICanvasIMGUI> m_UICanvas = nullptr;

		bool m_isRunning = false;

	protected:
		//std::queue<WindowEvent> m_userEvent;
	};

	Application* createApplication();
}
