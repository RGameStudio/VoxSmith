#pragma once

#include "../Core.hpp"

struct GLFWwindow;

namespace VoxSmith
{
	class VOX_SMITH_API UICanvasIMGUI
	{
	public:
		UICanvasIMGUI(GLFWwindow* window);
		~UICanvasIMGUI() = default;

		void update();

		UICanvasIMGUI() = delete;
		UICanvasIMGUI(const UICanvasIMGUI&) = delete;
		UICanvasIMGUI(UICanvasIMGUI&&) = delete;
		UICanvasIMGUI& operator=(const UICanvasIMGUI&) = delete;
		UICanvasIMGUI& operator=(UICanvasIMGUI&&) = delete;

	private:
		GLFWwindow* m_windowPtr;
	};
}