#include "app_window.h"
#include "top_panel.h"
#include "bottom_panel.h"
#include "project.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <string>

static float topHeight    = 400.0f;
static const float dividerHeight = 6.0f;


static MGE::Project g_project;

static bool s_openProjectSettings = false;

static void RenderProjectSettingsModal(MGE::Project& project) {
    if (s_openProjectSettings) {
        ImGui::OpenPopup("Project Settings");
        s_openProjectSettings = false;
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(420, 0), ImGuiCond_Appearing);

    if (!ImGui::BeginPopupModal("Project Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        return;

    MGE::Timeline&        tl   = project.GetTimeline();
    const MGE::ProjectMetadata& meta = project.GetMetadata();

    ImGui::SeparatorText("Identity");

    
    static char nameBuf[128] = "";
    static int  lastVersion  = -1;
    if (lastVersion != project.GetVersion()) {
        strncpy_s(nameBuf, sizeof(nameBuf), project.GetName().c_str(), _TRUNCATE);
        lastVersion = project.GetVersion();
    }
    ImGui::SetNextItemWidth(280);
    if (ImGui::InputText("Name", nameBuf, sizeof(nameBuf), ImGuiInputTextFlags_EnterReturnsTrue)) {
        project.SetName(std::string(nameBuf));
        project.BumpVersion();
        lastVersion = project.GetVersion();
    }
    ImGui::SameLine();
    ImGui::TextDisabled("v%d", project.GetVersion());

    ImGui::SeparatorText("Canvas");

    
    int res[2] = { meta.resolution.width, meta.resolution.height };
    ImGui::SetNextItemWidth(180);
    if (ImGui::DragInt2("Resolution", res, 1.0f, 1, 16384, "%d px")) {
        project.SetResolution(res[0], res[1]);
        project.BumpVersion();
    }

    
    const char* csItems[] = { "sRGB", "Linear RGB", "ACES" };
    int csIdx = static_cast<int>(meta.colorSpace);
    ImGui::SetNextItemWidth(180);
    if (ImGui::Combo("Color Space", &csIdx, csItems, IM_ARRAYSIZE(csItems))) {
        project.SetColorSpace(static_cast<MGE::ColorSpace>(csIdx));
        project.BumpVersion();
    }

    
    float us = meta.unitScale;
    ImGui::SetNextItemWidth(180);
    if (ImGui::DragFloat("Unit Scale", &us, 0.01f, 0.001f, 1000.0f, "%.3f px/unit")) {
        project.SetUnitScale(us);
        project.BumpVersion();
    }

    ImGui::SeparatorText("Timeline");

    
    float fpsF = (float)tl.getFPS();
    ImGui::SetNextItemWidth(120);
    if (ImGui::DragFloat("FPS", &fpsF, 1.0f, 1.0f, 240.0f, "%.0f"))
        tl.setFPS((double)fpsF);

    
    float startF = (float)tl.getStartTime();
    float endF   = (float)tl.getEndTime();
    ImGui::SetNextItemWidth(120);
    if (ImGui::DragFloat("Start (s)", &startF, 0.1f, 0.0f, (float)(tl.getEndTime() - 0.1)))
        tl.setStartTime((double)startF);
    ImGui::SetNextItemWidth(120);
    if (ImGui::DragFloat("End (s)",   &endF,   0.1f, (float)(tl.getStartTime() + 0.1), 3600.0f))
        tl.setEndTime((double)endF);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    float btnW = 100.0f;
    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - btnW) * 0.5f + ImGui::GetCursorPosX());
    if (ImGui::Button("Close", ImVec2(btnW, 0)))
        ImGui::CloseCurrentPopup();

    ImGui::EndPopup();
}

namespace AppWindow {

    void Run() {
        glfwInit();
        GLFWwindow* window = glfwCreateWindow(1280, 720, "Motion Graphics Editor", nullptr, nullptr);
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

            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);

            ImGui::Begin("#FullscreenWindow", nullptr,
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoResize   |
                ImGuiWindowFlags_NoMove     |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoBringToFrontOnFocus |
                ImGuiWindowFlags_NoNavFocus |
                ImGuiWindowFlags_MenuBar
            );

            
            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("Project Settings...", "Ctrl+,"))
                        s_openProjectSettings = true;
                    ImGui::Separator();
                    if (ImGui::MenuItem("Exit"))
                        glfwSetWindowShouldClose(window, GLFW_TRUE);
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Edit")) {
                    ImGui::MenuItem("Undo", "Ctrl+Z", false, false);
                    ImGui::MenuItem("Redo", "Ctrl+Y", false, false);
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("View")) {
                    ImGui::MenuItem("Properties",  nullptr, true,  false);
                    ImGui::MenuItem("Timeline",    nullptr, true,  false);
                    ImGui::MenuItem("Preview",     nullptr, true,  false);
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            
            RenderProjectSettingsModal(g_project);

            float totalHeight = ImGui::GetContentRegionAvail().y;

            if (topHeight < 80.0f)                      topHeight = 80.0f;
            if (topHeight > totalHeight - 80.0f)        topHeight = totalHeight - 80.0f;

            TopPanel::Render(topHeight, g_project);

            ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.35f, 0.35f, 0.35f, 1));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.45f, 0.45f, 1));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.55f, 0.55f, 0.55f, 1));
            ImGui::Button("##divider", ImVec2(-1, dividerHeight));
            ImGui::PopStyleColor(3);

            if (ImGui::IsItemHovered())
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
            if (ImGui::IsItemActive())
                topHeight += ImGui::GetIO().MouseDelta.y;

            BottomPanel::Render(g_project);

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

}