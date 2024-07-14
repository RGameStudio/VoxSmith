#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "../Window/Window.hpp"

#include "UICanvasIMGUI.hpp"

using namespace VoxSmith;

const char* g_glVersion = "#version 430";

UICanvasIMGUI::UICanvasIMGUI(const std::shared_ptr<Window>& window)
    : m_window(window)
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(m_window->getInstance(), true);
	ImGui_ImplOpenGL3_Init(g_glVersion);
}

void UICanvasIMGUI::update()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO();

	// FrameRate
    {
		ImGui::Begin("Framerate & Frametime");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }

}

void UICanvasIMGUI::setCameraInfo(const glm::vec3& pos)
{
	float v;

	ImGui::Begin("Camera Info");
	ImGui::Text("Camera position { %.1f, %.1f, %.1f}", pos.x, pos.y, pos.z);
	ImGui::SliderFloat("Camera speed", &v, 0.0f, 100.0f);
	ImGui::End();
}

void UICanvasIMGUI::setChunkInfo(bool& cullingStatus)
{
	ImGui::Begin("Chunk Info");
	ImGui::Checkbox("Show edges", &m_showEdges);
	ImGui::Checkbox("Enable culling", &cullingStatus);
	ImGui::End();
}

void UICanvasIMGUI::render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
