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

	private:
		GLFWwindow* m_windowPtr;
	};
}