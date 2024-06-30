#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "../Core.hpp"

class Window;

namespace VoxSmith
{
	class VOX_SMITH_API UICanvasIMGUI final
	{
	public:
		UICanvasIMGUI(const std::shared_ptr<Window>& window);
		~UICanvasIMGUI() = default;

		bool getEdgesRenderStatus() const { return m_showEdges; }

		void setCameraInfo(const glm::vec3& pos);
		void update();
		void render();

	private:
		std::shared_ptr<Window> m_window;

		bool m_showEdges = false;
	};
}