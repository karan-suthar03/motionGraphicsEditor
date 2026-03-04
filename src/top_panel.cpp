#include "top_panel.h"
#include "core/project.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <string>

namespace TopPanel {

    
    static const char* ColorSpaceName(MGE::ColorSpace cs) {
        switch (cs) {
            case MGE::ColorSpace::sRGB:      return "sRGB";
            case MGE::ColorSpace::LinearRGB: return "Linear RGB";
            case MGE::ColorSpace::ACES:      return "ACES";
        }
        return "Unknown";
    }

    static float splitX = 350.0f; 
    static const float dividerWidth = 6.0f;

    void Render(float height, MGE::Project& project) {
        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::BeginChild("TopPanel", ImVec2(0, height), false);
        ImGui::PopStyleVar();

        ImVec2 panelSize = ImGui::GetContentRegionAvail();

        const float minLeft = 150.0f;
        const float minRight = 200.0f;
        if (splitX < minLeft) splitX = minLeft;
        if (splitX > panelSize.x - minRight - dividerWidth)
            splitX = panelSize.x - minRight - dividerWidth;

        float rightWidth = panelSize.x - splitX - dividerWidth;

        
        const ImU32 colHeaderBg = IM_COL32(30, 30, 33, 255);
        const float headerHeight = 28.0f;

        
        ImVec2 startCursorPos = ImGui::GetCursorPos();

        
        
        
        ImGui::SetCursorPos(startCursorPos);
        ImGui::BeginChild("TopLeftPane", ImVec2(splitX, panelSize.y), false);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        
        ImGui::BeginChild("TopLeftHeader", ImVec2(splitX, headerHeight), false, ImGuiWindowFlags_NoScrollbar);
        ImDrawList* leftHeaderDraw = ImGui::GetWindowDrawList();
        ImVec2 leftHeaderPos = ImGui::GetCursorScreenPos();

        leftHeaderDraw->AddRectFilled(leftHeaderPos, ImVec2(leftHeaderPos.x + splitX, leftHeaderPos.y + headerHeight), colHeaderBg);
        leftHeaderDraw->AddText(ImVec2(leftHeaderPos.x + 10, leftHeaderPos.y + 6), IM_COL32(180, 180, 180, 255), "Properties");
        ImGui::EndChild();

        
        ImGui::BeginChild("TopLeftBody", ImVec2(splitX, panelSize.y - headerHeight), false);

        
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 8, ImGui::GetCursorPosY() + 8));
        ImGui::PushItemWidth(splitX - 100.0f); 

        
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            static float pos[2] = { 960.0f, 540.0f };
            static float scale = 100.0f;
            static float rotation = 0.0f;
            static float opacity = 100.0f;

            ImGui::Spacing();
            ImGui::DragFloat2("Position", pos, 1.0f);
            ImGui::DragFloat("Scale", &scale, 0.5f, 0.0f, 1000.0f, "%.1f %%");
            ImGui::DragFloat("Rotation", &rotation, 0.5f, -360.0f, 360.0f, "%.1f deg");
            ImGui::DragFloat("Opacity", &opacity, 0.5f, 0.0f, 100.0f, "%.1f %%");
            ImGui::Spacing();
        }

        if (ImGui::CollapsingHeader("Effects", ImGuiTreeNodeFlags_DefaultOpen)) {
            static bool enableBlur = false;
            static float blurAmount = 10.0f;

            ImGui::Spacing();
            ImGui::Checkbox("Gaussian Blur", &enableBlur);
            if (!enableBlur) ImGui::BeginDisabled();
            ImGui::DragFloat("Bluriness", &blurAmount, 0.2f, 0.0f, 100.0f);
            if (!enableBlur) ImGui::EndDisabled();
        }

        ImGui::PopItemWidth();
        ImGui::EndChild(); 

        ImGui::PopStyleVar(); 
        ImGui::EndChild(); 


        
        
        
        
        ImGui::SetCursorPos(ImVec2(startCursorPos.x + splitX, startCursorPos.y));

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.20f, 0.22f, 1));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.35f, 0.35f, 0.38f, 1));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.45f, 0.45f, 0.48f, 1));

        ImGui::Button("##topDivider", ImVec2(dividerWidth, panelSize.y));

        ImGui::PopStyleColor(3);

        if (ImGui::IsItemHovered())
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        if (ImGui::IsItemActive())
            splitX += ImGui::GetIO().MouseDelta.x;


        
        
        
        ImGui::SetCursorPos(ImVec2(startCursorPos.x + splitX + dividerWidth, startCursorPos.y));
        ImGui::BeginChild("TopRightPane", ImVec2(rightWidth, panelSize.y), false);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        
        ImGui::BeginChild("TopRightHeader", ImVec2(rightWidth, headerHeight), false, ImGuiWindowFlags_NoScrollbar);
        ImDrawList* rightHeaderDraw = ImGui::GetWindowDrawList();
        ImVec2 rightHeaderPos = ImGui::GetCursorScreenPos();

        rightHeaderDraw->AddRectFilled(rightHeaderPos, ImVec2(rightHeaderPos.x + rightWidth, rightHeaderPos.y + headerHeight), colHeaderBg);
        {
            const MGE::ProjectMetadata& m = project.GetMetadata();
            char previewLabel[64];
            snprintf(previewLabel, sizeof(previewLabel), "Preview - %dx%d  |  %s  |  v%d",
                m.resolution.width, m.resolution.height,
                ColorSpaceName(m.colorSpace),
                project.GetVersion());
            rightHeaderDraw->AddText(ImVec2(rightHeaderPos.x + 10, rightHeaderPos.y + 6), IM_COL32(180, 180, 180, 255), previewLabel);
        }
        ImGui::EndChild();

        
        ImGui::BeginChild("TopRightBody", ImVec2(rightWidth, panelSize.y - headerHeight), false);
        ImDrawList* vpDraw = ImGui::GetWindowDrawList();
        ImVec2 vpPos = ImGui::GetCursorScreenPos();
        ImVec2 vpSize = ImGui::GetContentRegionAvail();

        
        vpDraw->AddRectFilled(vpPos, ImVec2(vpPos.x + vpSize.x, vpPos.y + vpSize.y), IM_COL32(20, 20, 22, 255));

        
        const float aspect = 16.0f / 9.0f;
        float canvasW = vpSize.x * 0.85f; 
        float canvasH = canvasW / aspect;

        if (canvasH > vpSize.y * 0.85f) { 
            canvasH = vpSize.y * 0.85f;
            canvasW = canvasH * aspect;
        }

        float offsetX = (vpSize.x - canvasW) * 0.5f;
        float offsetY = (vpSize.y - canvasH) * 0.5f;

        ImVec2 canvasMin(vpPos.x + offsetX, vpPos.y + offsetY);
        ImVec2 canvasMax(vpPos.x + offsetX + canvasW, vpPos.y + offsetY + canvasH);

        
        vpDraw->AddRectFilled(canvasMin, canvasMax, IM_COL32(10, 10, 10, 255));      
        vpDraw->AddRect(canvasMin, canvasMax, IM_COL32(80, 80, 80, 255), 0.0f, 0, 1.0f); 

        
        ImVec2 center(canvasMin.x + canvasW * 0.5f, canvasMin.y + canvasH * 0.5f);
        vpDraw->AddLine(ImVec2(center.x - 10, center.y), ImVec2(center.x + 10, center.y), IM_COL32(255, 255, 255, 100), 1.0f);
        vpDraw->AddLine(ImVec2(center.x, center.y - 10), ImVec2(center.x, center.y + 10), IM_COL32(255, 255, 255, 100), 1.0f);

        
        char zoomTxt[32];
        snprintf(zoomTxt, sizeof(zoomTxt), "Fit (%d%%)", (int)((canvasW / 1920.0f) * 100.0f));
        vpDraw->AddText(ImVec2(canvasMin.x + 5, canvasMax.y - 18), IM_COL32(150, 150, 150, 255), zoomTxt);

        ImGui::EndChild(); 

        ImGui::PopStyleVar(); 
        ImGui::EndChild(); 

        ImGui::EndChild(); 
    }

} 