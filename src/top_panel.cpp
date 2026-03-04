#include "top_panel.h"
#include "core/project.h"
#include "core/renderer.h"
#include "core/visual_layer.h"
#include "core/shape_layer.h"
#include "core/circle_layer.h"
#include <glad/glad.h>
#include "imgui.h"
#include "imgui_internal.h"
#include <string>
#include <cstring>
#include <algorithm>

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
    static MGE::Renderer g_renderer;
    static bool g_rendererReady = false;

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

        MGE::Layer* selLayer = nullptr;
        int selIdx = project.GetSelectedLayerIndex();
        const auto& sceneLayers = project.GetScene().getLayers();
        if (selIdx >= 0 && selIdx < (int)sceneLayers.size())
            selLayer = sceneLayers[selIdx].get();

        
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 8, ImGui::GetCursorPosY() + 8));
        ImGui::PushItemWidth(splitX - 110.0f); 

        if (!selLayer) {
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::TextDisabled("No layer selected");
            ImGui::Spacing();
            ImGui::TextDisabled("Click a layer in the timeline");
            ImGui::TextDisabled("to inspect its properties.");
        } else {
            char nameBuf[128];
            strncpy_s(nameBuf, sizeof(nameBuf), selLayer->getName().c_str(), _TRUNCATE);
            if (ImGui::InputText("Name", nameBuf, sizeof(nameBuf)))
                selLayer->setName(nameBuf);

            bool visible = selLayer->isVisible();
            if (ImGui::Checkbox("Visible", &visible))
                selLayer->setVisible(visible);

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (auto* vl = dynamic_cast<MGE::VisualLayer*>(selLayer)) {
                auto& tf = vl->getTransform();

                if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::Spacing();
                    float pos[2] = { tf.x, tf.y };
                    if (ImGui::DragFloat2("Position", pos, 1.0f)) { tf.x = pos[0]; tf.y = pos[1]; }
                    float scaleXPct = tf.scaleX * 100.0f;
                    float scaleYPct = tf.scaleY * 100.0f;
                    if (ImGui::DragFloat("Scale X", &scaleXPct, 0.5f, 0.0f, 1000.0f, "%.1f %%")) tf.scaleX = scaleXPct / 100.0f;
                    if (ImGui::DragFloat("Scale Y", &scaleYPct, 0.5f, 0.0f, 1000.0f, "%.1f %%")) tf.scaleY = scaleYPct / 100.0f;
                    ImGui::DragFloat("Rotation", &tf.rotation, 0.5f, -360.0f, 360.0f, "%.1f deg");
                    float opacity = vl->getOpacity() * 100.0f;
                    if (ImGui::DragFloat("Opacity", &opacity, 0.5f, 0.0f, 100.0f, "%.1f %%"))
                        vl->setOpacity(opacity / 100.0f);
                    ImGui::Spacing();
                }

                if (auto* sl = dynamic_cast<MGE::ShapeLayer*>(selLayer)) {
                    if (ImGui::CollapsingHeader("Shape", ImGuiTreeNodeFlags_DefaultOpen)) {
                        ImGui::Spacing();
                        if (auto* cl = dynamic_cast<MGE::CircleLayer*>(sl)) {
                            float radius = cl->getRadius();
                            if (ImGui::DragFloat("Radius", &radius, 0.5f, 1.0f, 4000.0f, "%.1f px"))
                                cl->setRadius(radius);
                        }
                        MGE::Color fc = sl->getFillColor();
                        float col[4] = { fc.r, fc.g, fc.b, fc.a };
                        if (ImGui::ColorEdit4("Fill Color", col))
                            sl->setFillColor({ col[0], col[1], col[2], col[3] });
                        ImGui::Spacing();
                    }
                }

                if (ImGui::CollapsingHeader("Effects", ImGuiTreeNodeFlags_DefaultOpen)) {
                    static bool enableBlur = false;
                    static float blurAmount = 10.0f;

                    ImGui::Spacing();
                    ImGui::Checkbox("Gaussian Blur", &enableBlur);
                    if (!enableBlur) ImGui::BeginDisabled();
                    ImGui::DragFloat("Blurriness", &blurAmount, 0.2f, 0.0f, 100.0f);
                    if (!enableBlur) ImGui::EndDisabled();
                    ImGui::Spacing();
                }
            } else {
                ImGui::Spacing();
                ImGui::TextDisabled("(Audio layer)");
                ImGui::TextDisabled("No visual properties.");
            }
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

        
        const MGE::ProjectMetadata& meta = project.GetMetadata();
        const float aspect = (float)meta.resolution.width / (float)meta.resolution.height;
        float canvasW = vpSize.x * 0.85f; 
        float canvasH = canvasW / aspect;

        if (canvasH > vpSize.y * 0.85f) { 
            canvasH = vpSize.y * 0.85f;
            canvasW = canvasH * aspect;
        }

        ImVec2 canvasMin(vpPos.x + (vpSize.x - canvasW) * 0.5f, vpPos.y + (vpSize.y - canvasH) * 0.5f);
        ImVec2 canvasMax(canvasMin.x + canvasW, canvasMin.y + canvasH);
        const int previewW = std::max(1, (int)canvasW);
        const int previewH = std::max(1, (int)canvasH);
        if (!g_rendererReady) {
            g_renderer.init(previewW, previewH);
            g_rendererReady = true;
        } else if (g_renderer.getWidth() != previewW || g_renderer.getHeight() != previewH) {
            g_renderer.resize(previewW, previewH);
        }
        g_renderer.setProjectionSize(meta.resolution.width, meta.resolution.height);
        
        MGE::Time currentTime = project.GetTimeline().getCurrentTime();
        g_renderer.beginFrame(meta.backgroundColor);
        project.GetScene().renderFrame(currentTime, g_renderer);
        g_renderer.endFrame();
        
        vpDraw->AddRect(canvasMin, canvasMax, IM_COL32(80, 80, 80, 255), 0.0f, 0, 1.0f);
        ImGui::SetCursorScreenPos(canvasMin);
        ImGui::Image(
            (ImTextureID)(uintptr_t)g_renderer.getTexture(),
            ImVec2(canvasW, canvasH),
            ImVec2(0, 1), ImVec2(1, 0)  
        );
        
        char zoomTxt[32];
        snprintf(zoomTxt, sizeof(zoomTxt), "Fit (%d%%)", (int)((canvasW / (float)meta.resolution.width) * 100.0f));
        vpDraw->AddText(ImVec2(canvasMin.x + 5, canvasMax.y - 18), IM_COL32(150, 150, 150, 255), zoomTxt);

        ImGui::EndChild(); 

        ImGui::PopStyleVar(); 
        ImGui::EndChild(); 

        ImGui::EndChild(); 
    }

} 