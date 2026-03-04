#include "bottom_panel.h"
#include "core/project.h"
#include "core/layer.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <cmath>
#include <memory>
#include <algorithm>
#include "add_layer_popup.h"

namespace BottomPanel {
    static float splitX = 250.0f;
    static const float dividerWidth = 6.0f;
    
    static const float rowHeight = 34.0f;
    static const float headerHeight = 28.0f;
    static const float scrollbarHeight = 18.0f; 

    static float sharedScrollY = 0.0f;
    static bool leftPaneScrolled = false;
    static float projectDuration = 60.0f;
    
    static float pixelsPerSecond = 50.0f;
    static bool isPanning = false;

    void Render(MGE::Project& project) {
        MGE::Timeline& timeline = project.GetTimeline();
        MGE::Scene&    scene    = project.GetScene();

        
        projectDuration = (float)timeline.getDuration().seconds;
        if (projectDuration < 1.0f) projectDuration = 1.0f;
        float playheadTime = (float)timeline.getCurrentTime().seconds;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::BeginChild("BottomPanel", ImVec2(0, 0), false);
        ImGui::PopStyleVar();

        ImVec2 panelSize = ImGui::GetContentRegionAvail();

        const float minLeft = 120.0f;
        const float minRight = 120.0f;
        if (splitX < minLeft) splitX = minLeft;
        if (splitX > panelSize.x - minRight - dividerWidth)
            splitX = panelSize.x - minRight - dividerWidth;

        float rightWidth = panelSize.x - splitX - dividerWidth;
        if (rightWidth <= 0.0f) rightWidth = 1.0f;

        float visibleStart = playheadTime - (rightWidth / 2.0f) / pixelsPerSecond;
        float visibleEnd   = playheadTime + (rightWidth / 2.0f) / pixelsPerSecond;

        const ImU32 colHeaderBg = IM_COL32(30, 30, 33, 255);
        const ImU32 colTrackEven = IM_COL32(38, 38, 42, 255);
        const ImU32 colTrackOdd = IM_COL32(43, 43, 48, 255);
        const ImU32 colGridLine = IM_COL32(70, 70, 75, 100);
        const ImU32 colPlayhead = IM_COL32(240, 50, 80, 255);

        ImVec2 startCursorPos = ImGui::GetCursorPos();

        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)) {
            if (ImGui::GetIO().KeyCtrl) {
                float zoomDelta = ImGui::GetIO().MouseWheel;
                if (zoomDelta != 0.0f) {
                    pixelsPerSecond *= (1.0f + zoomDelta * 0.1f);
                    if (pixelsPerSecond < 1.0f) pixelsPerSecond = 1.0f;
                    if (pixelsPerSecond > 2000.0f) pixelsPerSecond = 2000.0f;
                    
                    visibleStart = playheadTime - (rightWidth / 2.0f) / pixelsPerSecond;
                    visibleEnd   = playheadTime + (rightWidth / 2.0f) / pixelsPerSecond;
                }
            } else {
                float wheel = ImGui::GetIO().MouseWheel;
                if (wheel != 0.0f) {
                    const auto& layers = scene.getLayers();
                    float clipsViewHeight = panelSize.y - headerHeight - scrollbarHeight;
                    float maxScroll = (layers.size() * rowHeight) - clipsViewHeight;
                    if (maxScroll < 0.0f) maxScroll = 0.0f;
                    sharedScrollY -= wheel * rowHeight;
                    if (sharedScrollY < 0.0f) sharedScrollY = 0.0f;
                    if (sharedScrollY > maxScroll) sharedScrollY = maxScroll;
                    leftPaneScrolled = true;
                }
            }
        }

        ImGui::SetCursorPos(ImVec2(startCursorPos.x + splitX + dividerWidth, startCursorPos.y));
        ImGui::BeginChild("RightPane", ImVec2(rightWidth, panelSize.y), false, ImGuiWindowFlags_NoScrollbar);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        ImVec2 rightPaneScreenPos = ImGui::GetCursorScreenPos();

        
        ImGui::BeginChild("RightRuler", ImVec2(rightWidth, headerHeight), false, ImGuiWindowFlags_NoScrollbar);
        ImDrawList* rulerDraw = ImGui::GetWindowDrawList();
        ImVec2 rulerPos = ImGui::GetCursorScreenPos();

        rulerDraw->AddRectFilled(rulerPos, ImVec2(rulerPos.x + rightWidth, rulerPos.y + headerHeight), colHeaderBg);

        float step = 1.0f;
        if (pixelsPerSecond < 5.0f) step = 10.0f;
        else if (pixelsPerSecond < 15.0f) step = 5.0f;
        else if (pixelsPerSecond < 30.0f) step = 2.0f;

        float startTick = std::floor(visibleStart / step) * step;

        for (float t = startTick; t <= visibleEnd; t += step) {
            if (t < 0.0f) continue; 
            float x = rulerPos.x + (t - visibleStart) * pixelsPerSecond;
            if (x >= rulerPos.x && x <= rulerPos.x + rightWidth) {
                rulerDraw->AddLine(ImVec2(x, rulerPos.y + headerHeight - 8), ImVec2(x, rulerPos.y + headerHeight), IM_COL32(150, 150, 150, 255));

                char timeStr[16];
                snprintf(timeStr, sizeof(timeStr), "00:%02d:%02d", (int)(t / 60), (int)t % 60);
                rulerDraw->AddText(ImVec2(x + 4, rulerPos.y + 4), IM_COL32(150, 150, 150, 255), timeStr);
            }
        }
        ImGui::EndChild();

        
        
        if (leftPaneScrolled) {
            ImGui::SetNextWindowScroll(ImVec2(0.0f, sharedScrollY));
            leftPaneScrolled = false;
        }
        ImGui::BeginChild("RightClips", ImVec2(rightWidth, panelSize.y - headerHeight - scrollbarHeight), false,
            ImGuiWindowFlags_AlwaysVerticalScrollbar);
        ImDrawList* rightDraw = ImGui::GetWindowDrawList();
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        ImVec2 bgPos = ImGui::GetCursorScreenPos();

        const auto& layers = scene.getLayers();
        float totalHeight = (int)layers.size() * rowHeight;

        for (float t = startTick; t <= visibleEnd; t += step) {
            if (t < 0.0f) continue;
            float x = bgPos.x + (t - visibleStart) * pixelsPerSecond;
            if (x >= bgPos.x && x <= bgPos.x + rightWidth) {
                rightDraw->AddLine(ImVec2(x, bgPos.y), ImVec2(x, bgPos.y + totalHeight), colGridLine);
            }
        }

        for (int i = (int)layers.size() - 1, row = 0; i >= 0; i--, row++) {
            ImVec2 cursorP = ImGui::GetCursorScreenPos();

            ImU32 bgCol = (row % 2 == 0) ? colTrackEven : colTrackOdd;
            rightDraw->AddRectFilled(cursorP, ImVec2(cursorP.x + rightWidth, cursorP.y + rowHeight), bgCol);
            rightDraw->AddLine(ImVec2(cursorP.x, cursorP.y + rowHeight), ImVec2(cursorP.x + rightWidth, cursorP.y + rowHeight), IM_COL32(20, 20, 20, 255));

            
            float clipStart = (float)layers[i]->getStartTime().seconds;
            float clipEnd   = (float)(layers[i]->getStartTime().seconds + layers[i]->getDuration().seconds);

            float x0 = cursorP.x + (clipStart - visibleStart) * pixelsPerSecond;
            float x1 = cursorP.x + (clipEnd - visibleStart) * pixelsPerSecond;

            
            if (x1 > cursorP.x && x0 < cursorP.x + rightWidth) {
                
                float drawX0 = (x0 < cursorP.x) ? cursorP.x : x0;
                float drawX1 = (x1 > cursorP.x + rightWidth) ? cursorP.x + rightWidth : x1;

                ImU32 colBase = IM_COL32(255, 20, 20, 255);
                ImVec4 colV = ImGui::ColorConvertU32ToFloat4(colBase);
                ImU32 colDark = ImGui::ColorConvertFloat4ToU32(ImVec4(colV.x * 0.7f, colV.y * 0.7f, colV.z * 0.7f, 1.0f));

                const float padding = 3.0f;
                ImVec2 clipMin(drawX0, cursorP.y + padding);
                ImVec2 clipMax(drawX1, cursorP.y + rowHeight - padding);
                float rounding = 4.0f;

                rightDraw->AddRectFilled(clipMin, clipMax, colDark, rounding);
                rightDraw->AddRectFilled(clipMin, ImVec2(clipMax.x, clipMin.y + 6), colBase, rounding, ImDrawFlags_RoundCornersTop);
                rightDraw->AddRect(clipMin, clipMax, IM_COL32(20, 20, 20, 255), rounding);

                rightDraw->PushClipRect(clipMin, clipMax, true);
                char clipName[32];
                snprintf(clipName, sizeof(clipName), "Clip %02d", i + 1);
                rightDraw->AddText(ImVec2(clipMin.x + 6, clipMin.y + 8), IM_COL32(255, 255, 255, 255), clipName);
                rightDraw->PopClipRect();
            }

            ImGui::Dummy(ImVec2(0.0f, rowHeight));
        }

        ImGui::PopStyleVar();
        sharedScrollY = ImGui::GetScrollY();
        ImGui::EndChild(); 

        
        ImGui::BeginChild("ZoomScrollbar", ImVec2(rightWidth, scrollbarHeight), false, ImGuiWindowFlags_NoScrollbar);
        ImDrawList* sbDraw = ImGui::GetWindowDrawList();
        ImVec2 sbPos = ImGui::GetCursorScreenPos();

        
        sbDraw->AddRectFilled(sbPos, ImVec2(sbPos.x + rightWidth, sbPos.y + scrollbarHeight), IM_COL32(25, 25, 28, 255));

        
        ImGui::InvisibleButton("##ZoomScrollbarArea", ImVec2(rightWidth, scrollbarHeight));
        bool sbHovered = ImGui::IsItemHovered();
        bool sbActive = ImGui::IsItemActive();

        float scrollMin = 0.0f;
        float scrollMax = std::max(std::max(projectDuration, playheadTime), 1.0f);

        const float fixedHandleW = 60.0f;
        float usableWidth = std::max(rightWidth - fixedHandleW, 1.0f);

        float scrollFraction = std::clamp((playheadTime - scrollMin) / (scrollMax - scrollMin), 0.0f, 1.0f);
        float handleL = sbPos.x + scrollFraction * usableWidth;
        float handleR = handleL + fixedHandleW;

        if (sbActive) {
            if (ImGui::IsItemActivated()) {
                float clickX = ImGui::GetMousePos().x - sbPos.x;
                scrollFraction = std::clamp((clickX - fixedHandleW * 0.5f) / usableWidth, 0.0f, 1.0f);
                float newPlayhead = scrollMin + scrollFraction * (scrollMax - scrollMin);
                timeline.scrub(MGE::Time{newPlayhead});
                playheadTime = newPlayhead;
            } else {
                float deltaFraction = ImGui::GetIO().MouseDelta.x / usableWidth;
                float deltaT = deltaFraction * (scrollMax - scrollMin);
                float newPlayhead = playheadTime + deltaT;
                
                if (newPlayhead < scrollMin) newPlayhead = scrollMin;
                if (newPlayhead > scrollMax) newPlayhead = scrollMax;

                timeline.scrub(MGE::Time{newPlayhead});
                playheadTime = newPlayhead;
            }

            visibleStart = playheadTime - (rightWidth / 2.0f) / pixelsPerSecond;
            visibleEnd   = playheadTime + (rightWidth / 2.0f) / pixelsPerSecond;

            scrollFraction = std::clamp((playheadTime - scrollMin) / (scrollMax - scrollMin), 0.0f, 1.0f);
            handleL = sbPos.x + scrollFraction * usableWidth;
            handleR = handleL + fixedHandleW;
        }

        if (sbHovered || sbActive)
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        ImU32 handleCol = sbActive  ? IM_COL32(150, 150, 160, 255)
                        : sbHovered ? IM_COL32(120, 120, 130, 255)
                                    : IM_COL32(80,  80,  85,  255);
        sbDraw->AddRectFilled(ImVec2(handleL, sbPos.y + 3), ImVec2(handleR, sbPos.y + scrollbarHeight - 3), handleCol, 4.0f);

        ImGui::EndChild(); 
        ImGui::PopStyleVar(); 
        ImGui::EndChild(); 


        
        
        
        ImGui::SetCursorPos(startCursorPos);
        ImGui::BeginChild("LeftPane", ImVec2(splitX, panelSize.y), false, ImGuiWindowFlags_NoScrollbar);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        

        
        ImGui::BeginChild("LeftHeader", ImVec2(splitX, headerHeight), false, ImGuiWindowFlags_NoScrollbar);
        ImDrawList* leftHeaderDraw = ImGui::GetWindowDrawList();
        ImVec2 leftHeaderPos = ImGui::GetCursorScreenPos();

        leftHeaderDraw->AddRectFilled(leftHeaderPos, ImVec2(leftHeaderPos.x + splitX, leftHeaderPos.y + headerHeight), colHeaderBg);
        leftHeaderDraw->AddText(ImVec2(leftHeaderPos.x + 10, leftHeaderPos.y + 6), IM_COL32(180, 180, 180, 255), "Tracks");

        
        {
            const float btnW = 52.0f;
            ImGui::SetCursorScreenPos(ImVec2(leftHeaderPos.x + splitX - btnW - 6, leftHeaderPos.y + 4));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 2));
            if (ImGui::SmallButton("+ Layer")) {
				ImGui::OpenPopup("AddLayerMenuPopup");
            }

            AddLayerPopup::Render(scene, playheadTime);

            ImGui::PopStyleVar();
        }
        ImGui::EndChild(); 

        
        ImGui::SetNextWindowScroll(ImVec2(0.0f, sharedScrollY));
        ImGui::BeginChild("LeftTracks", ImVec2(splitX, panelSize.y - headerHeight - scrollbarHeight), false,
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        ImDrawList* leftDraw = ImGui::GetWindowDrawList();
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        for (int i = (int)layers.size() - 1, row = 0; i >= 0; i--, row++) {
            ImVec2 cursorP = ImGui::GetCursorScreenPos();

            ImU32 bgCol = (row % 2 == 0) ? colTrackEven : colTrackOdd;
            leftDraw->AddRectFilled(cursorP, ImVec2(cursorP.x + splitX, cursorP.y + rowHeight), bgCol);

            leftDraw->AddRectFilled(cursorP, ImVec2(cursorP.x + 4, cursorP.y + rowHeight), IM_COL32(100, 180, 255, 255));
            leftDraw->AddLine(ImVec2(cursorP.x, cursorP.y + rowHeight), ImVec2(cursorP.x + splitX, cursorP.y + rowHeight), IM_COL32(20, 20, 20, 255));
            leftDraw->AddText(ImVec2(cursorP.x + 15, cursorP.y + (rowHeight - ImGui::GetTextLineHeight()) * 0.5f), IM_COL32(220, 220, 220, 255), layers[i]->getName().c_str());

            ImGui::Dummy(ImVec2(0.0f, rowHeight));
        }

        ImGui::PopStyleVar();
        ImGui::EndChild(); 

        
        ImGui::BeginChild("LeftBottomEmpty", ImVec2(splitX, scrollbarHeight), false, ImGuiWindowFlags_NoScrollbar);
        ImDrawList* lbDraw = ImGui::GetWindowDrawList();
        ImVec2 lbPos = ImGui::GetCursorScreenPos();
        lbDraw->AddRectFilled(lbPos, ImVec2(lbPos.x + splitX, lbPos.y + scrollbarHeight), colHeaderBg);
        ImGui::EndChild();

        ImGui::PopStyleVar(); 
        ImGui::EndChild(); 


        
        
        
        ImGui::SetCursorPos(ImVec2(startCursorPos.x + splitX, startCursorPos.y));

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.20f, 0.22f, 1));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.35f, 0.35f, 0.38f, 1));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.45f, 0.45f, 0.48f, 1));

        ImGui::Button("##VerticalDivider", ImVec2(dividerWidth, panelSize.y));

        ImGui::PopStyleColor(3);

        if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        if (ImGui::IsItemActive()) splitX += ImGui::GetIO().MouseDelta.x;
        


        ImGuiIO& io = ImGui::GetIO();
        ImVec2 mouse = io.MousePos;
        bool inTimelineArea = mouse.x >= rightPaneScreenPos.x && mouse.x <= rightPaneScreenPos.x + rightWidth
                           && mouse.y >= rightPaneScreenPos.y && mouse.y <= rightPaneScreenPos.y + panelSize.y - scrollbarHeight;

        if (inTimelineArea && ImGui::IsMouseClicked(0)) {
            isPanning = true;
        }
        if (ImGui::IsMouseReleased(0)) {
            isPanning = false;
        }

        if (isPanning) {
            float deltaX = io.MouseDelta.x;
            if (deltaX != 0.0f) {
                float deltaT = -deltaX / pixelsPerSecond;
                float newPlayhead = playheadTime + deltaT;
                if (newPlayhead < 0.0f) newPlayhead = 0.0f;
                
                timeline.scrub(MGE::Time{newPlayhead});
            }
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
        } else if (inTimelineArea && !sbHovered && !sbActive) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        }

        float phX = rightPaneScreenPos.x + (rightWidth / 2.0f);
        float phYTop = rightPaneScreenPos.y;
        float phYBot = rightPaneScreenPos.y + panelSize.y - scrollbarHeight;

        ImDrawList* overlayDraw = ImGui::GetForegroundDrawList();
        overlayDraw->PushClipRect(rightPaneScreenPos, ImVec2(rightPaneScreenPos.x + rightWidth, rightPaneScreenPos.y + panelSize.y), true);

        overlayDraw->AddLine(ImVec2(phX, phYTop), ImVec2(phX, phYBot), colPlayhead, 2.0f);
        overlayDraw->AddTriangleFilled(ImVec2(phX - 6, phYTop), ImVec2(phX + 6, phYTop), ImVec2(phX, phYTop + 10), colPlayhead);

        overlayDraw->PopClipRect();

        ImGui::EndChild(); 
    }
}