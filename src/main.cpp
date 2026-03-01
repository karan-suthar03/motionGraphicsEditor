#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

static float topHeight = 200.0f;
static const float dividerHeight = 6.0f;

int main() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui Fullscreen Split", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ── FULLSCREEN WINDOW SETUP ────────────
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);

        ImGui::Begin("#FullscreenWindow", nullptr,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus
        );

        float totalHeight = ImGui::GetContentRegionAvail().y;

        // Clamp top panel
        if (topHeight < 80.0f) topHeight = 80.0f;
        if (topHeight > totalHeight - 80.0f)
            topHeight = totalHeight - 80.0f;

        // ── TOP PANEL ─────────────────────────
        ImGui::BeginChild("TopPanel", ImVec2(0, topHeight), true);
        ImGui::Text("Top Panel");
        ImGui::Text("Drag divider below");
        ImGui::EndChild();

        // ── DIVIDER ───────────────────────────
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.35f, 0.35f, 0.35f, 1));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.45f, 0.45f, 1));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.55f, 0.55f, 0.55f, 1));

        ImGui::Button("##divider", ImVec2(-1, dividerHeight));
        ImGui::PopStyleColor(3);

        if (ImGui::IsItemHovered())
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);

        if (ImGui::IsItemActive())
            topHeight += ImGui::GetIO().MouseDelta.y;

        // ── BOTTOM PANEL ──────────────────────
        ImGui::BeginChild("BottomPanel", ImVec2(0, 0), true);
        ImGui::Text("Bottom Panel");
        ImGui::Text("Fullscreen, borderless layout");
        ImGui::EndChild();

        ImGui::End();

        ImGui::Render();
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}