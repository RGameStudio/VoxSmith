#pragma once

struct GLFWwindow;

namespace VoxSmith
{
	class UIWrapperIMGUI
	{
	public:
		UIWrapperIMGUI(GLFWwindow* window);
		~UIWrapperIMGUI() = default;

		void update();

		UIWrapperIMGUI() = delete;
		UIWrapperIMGUI(const UIWrapperIMGUI&) = delete;
		UIWrapperIMGUI(UIWrapperIMGUI&&) = delete;
		UIWrapperIMGUI& operator=(const UIWrapperIMGUI&) = delete;
		UIWrapperIMGUI& operator=(UIWrapperIMGUI&&) = delete;

	private:
		GLFWwindow* m_windowPtr;
	};
}