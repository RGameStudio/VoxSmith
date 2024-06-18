#pragma once

#include <memory>

#include "../Core.hpp"

class Window;

namespace VoxSmith
{
	class VOX_SMITH_API UICanvasIMGUI final
	{
	public:
		UICanvasIMGUI(const std::shared_ptr<Window>& window);
		~UICanvasIMGUI() = default;

		void update();

	private:
		std::shared_ptr<Window> m_window;
	};
}