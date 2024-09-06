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

		inline bool getEdgesRenderStatus() const { return m_showEdges; }
		inline bool getChunkOutlineStatus() const { return m_showOutline; }

		void setCameraInfo(const glm::vec3& pos);
		void setChunkInfo(bool& cullingStatus);

		void update();
		void render();

	private:
		std::shared_ptr<Window> m_window;

		bool m_showEdges = false;
		bool m_showOutline = false;
	};
}