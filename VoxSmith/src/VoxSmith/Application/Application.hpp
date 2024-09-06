#pragma once

#include <queue>
#include <memory>

#include "VoxSmith/Core.hpp"
#include "VoxSmith/Window/Window.hpp"
#include "VoxSmith/Camera/Camera.hpp"
#include "VoxSmith/UICanvasIMGUI/UICanvasIMGUI.hpp"
#include "VoxSmith/Renderer/Renderer.hpp"

namespace VoxSmith
{
	struct WindowEvent;

	class VOX_SMITH_API Application
	{
	public:
		Application(const uint32_t width, const uint32_t height);
		virtual ~Application() noexcept;

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
		std::unique_ptr<UICanvasIMGUI> m_UICanvas = nullptr;

		bool m_isRunning = false;

	protected:
		std::unique_ptr<Camera> m_camera = nullptr;
		std::shared_ptr<Renderer> m_renderer = nullptr;

		// @NOTE: these functions are derived in Sandbox where the client code is allowed
		virtual void update(const float dt);
		virtual void draw(const float dt, const float cframe);
		virtual void updateCamera(const float dt);

		bool chunkOutlineStatus() { return m_UICanvas->getChunkOutlineStatus(); }
	};

	Application* createApplication();
}
