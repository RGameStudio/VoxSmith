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
    io.DisplaySize = ImVec2(800.0f, 800.0f);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        ImGui::Begin("Framerate & Frametime");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
