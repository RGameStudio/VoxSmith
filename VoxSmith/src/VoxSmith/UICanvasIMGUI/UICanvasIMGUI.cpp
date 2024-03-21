#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "UICanvasIMGUI.hpp"

using namespace VoxSmith;

const char* g_glVersion = "#version 410";

UICanvasIMGUI::UICanvasIMGUI(GLFWwindow* window)
	: m_windowPtr(window)
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(m_windowPtr, true);
	ImGui_ImplOpenGL3_Init(g_glVersion);
}

void UICanvasIMGUI::update()
{
    static bool showSecondWindow = true;
	static bool showFirstWindow = true;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(800.0f, 800.0f);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &showFirstWindow);       // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &showSecondWindow);

        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        {
            counter++;
        }
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (showSecondWindow)
    {
        ImGui::Begin("Another Window", &showSecondWindow);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
        {
            showSecondWindow = false;
        }
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
