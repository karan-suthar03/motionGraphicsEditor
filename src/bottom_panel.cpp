#include "bottom_panel.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <cmath>

namespace BottomPanel {
    static float splitX = 250.0f;
    static const float dividerWidth = 6.0f;

    struct TrackDef {
        const char* name;
        ImU32 color;
    };

    static const TrackDef tracks[] = {
        {"Video 02",   IM_COL32(190, 100, 220, 255)},
        {"Video 01",   IM_COL32(100, 180, 255, 255)},
        {"Text",       IM_COL32(230, 160,  50, 255)},
        {"Effects",    IM_COL32(70, 210, 150, 255)},
        {"Audio 01",   IM_COL32(220,  80, 100, 255)},
        {"Audio 02",   IM_COL32(220,  80, 100, 255)}
    };

    static const int layerCount = sizeof(tracks) / sizeof(tracks[0]);

    
    static const float rowHeight = 34.0f;
    static const float headerHeight = 28.0f;
    static const float scrollbarHeight = 18.0f; 

    static float sharedScrollY = 0.0f;
    static bool leftPaneScrolled = false;
    static float playheadTime = 2.4f;
    static bool playheadDragging = false;

    
    static float projectDuration = 60.0f; 
    static float visibleStart = 0.0f;
    static float visibleEnd = 10.0f;
    static int zoomDragState = 0; 

    void Render() {
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

        
        const ImU32 colHeaderBg = IM_COL32(30, 30, 33, 255);
        const ImU32 colTrackEven = IM_COL32(38, 38, 42, 255);
        const ImU32 colTrackOdd = IM_COL32(43, 43, 48, 255);
        const ImU32 colGridLine = IM_COL32(70, 70, 75, 100);
        const ImU32 colPlayhead = IM_COL32(240, 50, 80, 255);

        ImVec2 startCursorPos = ImGui::GetCursorPos();

        
        
        
        ImGui::SetCursorPos(ImVec2(startCursorPos.x + splitX + dividerWidth, startCursorPos.y));
        ImGui::BeginChild("RightPane", ImVec2(rightWidth, panelSize.y), false, ImGuiWindowFlags_NoScrollbar);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        ImVec2 rightPaneScreenPos = ImGui::GetCursorScreenPos();
        float duration = visibleEnd - visibleStart;

        
        ImGui::BeginChild("RightRuler", ImVec2(rightWidth, headerHeight), false, ImGuiWindowFlags_NoScrollbar);
        ImDrawList* rulerDraw = ImGui::GetWindowDrawList();
        ImVec2 rulerPos = ImGui::GetCursorScreenPos();

        rulerDraw->AddRectFilled(rulerPos, ImVec2(rulerPos.x + rightWidth, rulerPos.y + headerHeight), colHeaderBg);

        
        float pixelsPerSecond = rightWidth / duration;
        float step = 1.0f;
        if (pixelsPerSecond < 5.0f) step = 10.0f;
        else if (pixelsPerSecond < 15.0f) step = 5.0f;
        else if (pixelsPerSecond < 30.0f) step = 2.0f;

        float startTick = std::floor(visibleStart / step) * step;

        for (float t = startTick; t <= visibleEnd; t += step) {
            float x = rulerPos.x + ((t - visibleStart) / duration) * rightWidth;
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
        float totalHeight = layerCount * rowHeight;

        
        for (float t = startTick; t <= visibleEnd; t += step) {
            float x = bgPos.x + ((t - visibleStart) / duration) * rightWidth;
            if (x >= bgPos.x && x <= bgPos.x + rightWidth) {
                rightDraw->AddLine(ImVec2(x, bgPos.y), ImVec2(x, bgPos.y + totalHeight), colGridLine);
            }
        }

        for (int i = 0; i < layerCount; i++) {
            ImVec2 cursorP = ImGui::GetCursorScreenPos();

            ImU32 bgCol = (i % 2 == 0) ? colTrackEven : colTrackOdd;
            rightDraw->AddRectFilled(cursorP, ImVec2(cursorP.x + rightWidth, cursorP.y + rowHeight), bgCol);
            rightDraw->AddLine(ImVec2(cursorP.x, cursorP.y + rowHeight), ImVec2(cursorP.x + rightWidth, cursorP.y + rowHeight), IM_COL32(20, 20, 20, 255));

            
            float clipStart = 1.0f + (i * 0.5f);
            float clipEnd = clipStart + 2.5f;

            float x0 = cursorP.x + ((clipStart - visibleStart) / duration) * rightWidth;
            float x1 = cursorP.x + ((clipEnd - visibleStart) / duration) * rightWidth;

            
            if (x1 > cursorP.x && x0 < cursorP.x + rightWidth) {
                
                float drawX0 = (x0 < cursorP.x) ? cursorP.x : x0;
                float drawX1 = (x1 > cursorP.x + rightWidth) ? cursorP.x + rightWidth : x1;

                ImU32 colBase = tracks[i].color;
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

        float handleL = sbPos.x + (visibleStart / projectDuration) * rightWidth;
        float handleR = sbPos.x + (visibleEnd / projectDuration) * rightWidth;
        float handleW = 10.0f; 

        if (sbActive) {
            float mx = ImGui::GetMousePos().x;
            if (ImGui::IsMouseClicked(0)) {
                if (std::abs(mx - handleL) <= handleW * 1.5f) zoomDragState = 1;
                else if (std::abs(mx - handleR) <= handleW * 1.5f) zoomDragState = 2;
                else if (mx > handleL && mx < handleR) zoomDragState = 3;
                else zoomDragState = 0;
            }

            float deltaT = (ImGui::GetIO().MouseDelta.x / rightWidth) * projectDuration;

            if (zoomDragState == 1) visibleStart += deltaT;
            else if (zoomDragState == 2) visibleEnd += deltaT;
            else if (zoomDragState == 3) {
                visibleStart += deltaT;
                visibleEnd += deltaT;
            }

            
            const float minZoom = 1.0f;
            if (visibleStart < 0.0f) {
                if (zoomDragState == 3) visibleEnd -= visibleStart;
                visibleStart = 0.0f;
            }
            if (visibleEnd > projectDuration) {
                if (zoomDragState == 3) visibleStart -= (visibleEnd - projectDuration);
                visibleEnd = projectDuration;
            }
            if (visibleStart < 0.0f) visibleStart = 0.0f;
            if (visibleEnd > projectDuration) visibleEnd = projectDuration;

            if (visibleEnd - visibleStart < minZoom) {
                if (zoomDragState == 1) visibleStart = visibleEnd - minZoom;
                if (zoomDragState == 2) visibleEnd = visibleStart + minZoom;
            }
        }
        else {
            zoomDragState = 0;
        }

        
        if (sbHovered || sbActive) {
            float mx = ImGui::GetMousePos().x;
            if (zoomDragState == 1 || (zoomDragState == 0 && std::abs(mx - handleL) <= handleW * 1.5f))
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
            else if (zoomDragState == 2 || (zoomDragState == 0 && std::abs(mx - handleR) <= handleW * 1.5f))
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
            else if (zoomDragState == 3 || (zoomDragState == 0 && mx > handleL && mx < handleR))
                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        }

        
        handleL = sbPos.x + (visibleStart / projectDuration) * rightWidth;
        handleR = sbPos.x + (visibleEnd / projectDuration) * rightWidth;

        
        sbDraw->AddRectFilled(ImVec2(handleL, sbPos.y + 3), ImVec2(handleR, sbPos.y + scrollbarHeight - 3), IM_COL32(70, 70, 75, 255), 4.0f);

        
        ImU32 handleCol = sbActive ? IM_COL32(150, 150, 160, 255) : IM_COL32(110, 110, 120, 255);
        sbDraw->AddRectFilled(ImVec2(handleL, sbPos.y + 3), ImVec2(handleL + handleW, sbPos.y + scrollbarHeight - 3), handleCol, 4.0f);
        sbDraw->AddRectFilled(ImVec2(handleR - handleW, sbPos.y + 3), ImVec2(handleR, sbPos.y + scrollbarHeight - 3), handleCol, 4.0f);

        
        sbDraw->AddLine(ImVec2(handleL + 4, sbPos.y + 6), ImVec2(handleL + 4, sbPos.y + scrollbarHeight - 6), IM_COL32(40, 40, 40, 255));
        sbDraw->AddLine(ImVec2(handleR - 4, sbPos.y + 6), ImVec2(handleR - 4, sbPos.y + scrollbarHeight - 6), IM_COL32(40, 40, 40, 255));

        ImGui::EndChild(); 
        ImGui::PopStyleVar(); 
        ImGui::EndChild(); 


        
        
        
        ImGui::SetCursorPos(startCursorPos);
        ImGui::BeginChild("LeftPane", ImVec2(splitX, panelSize.y), false, ImGuiWindowFlags_NoScrollbar);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        
        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)) {
            float wheel = ImGui::GetIO().MouseWheel;
            if (wheel != 0.0f) {
                float clipsViewHeight = panelSize.y - headerHeight - scrollbarHeight;
                float maxScroll = layerCount * rowHeight - clipsViewHeight;
                if (maxScroll < 0.0f) maxScroll = 0.0f;
                sharedScrollY -= wheel * rowHeight;
                if (sharedScrollY < 0.0f) sharedScrollY = 0.0f;
                if (sharedScrollY > maxScroll) sharedScrollY = maxScroll;
                leftPaneScrolled = true;
            }
        }

        
        ImGui::BeginChild("LeftHeader", ImVec2(splitX, headerHeight), false, ImGuiWindowFlags_NoScrollbar);
        ImDrawList* leftHeaderDraw = ImGui::GetWindowDrawList();
        ImVec2 leftHeaderPos = ImGui::GetCursorScreenPos();

        leftHeaderDraw->AddRectFilled(leftHeaderPos, ImVec2(leftHeaderPos.x + splitX, leftHeaderPos.y + headerHeight), colHeaderBg);
        leftHeaderDraw->AddText(ImVec2(leftHeaderPos.x + 10, leftHeaderPos.y + 6), IM_COL32(180, 180, 180, 255), "Tracks");
        ImGui::EndChild();

        
        ImGui::SetNextWindowScroll(ImVec2(0.0f, sharedScrollY));
        ImGui::BeginChild("LeftTracks", ImVec2(splitX, panelSize.y - headerHeight - scrollbarHeight), false,
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        ImDrawList* leftDraw = ImGui::GetWindowDrawList();
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        for (int i = 0; i < layerCount; i++) {
            ImVec2 cursorP = ImGui::GetCursorScreenPos();

            ImU32 bgCol = (i % 2 == 0) ? colTrackEven : colTrackOdd;
            leftDraw->AddRectFilled(cursorP, ImVec2(cursorP.x + splitX, cursorP.y + rowHeight), bgCol);

            leftDraw->AddRectFilled(cursorP, ImVec2(cursorP.x + 4, cursorP.y + rowHeight), tracks[i].color);
            leftDraw->AddLine(ImVec2(cursorP.x, cursorP.y + rowHeight), ImVec2(cursorP.x + splitX, cursorP.y + rowHeight), IM_COL32(20, 20, 20, 255));
            leftDraw->AddText(ImVec2(cursorP.x + 15, cursorP.y + (rowHeight - ImGui::GetTextLineHeight()) * 0.5f), IM_COL32(220, 220, 220, 255), tracks[i].name);

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


        
        
        

        
        float phX = rightPaneScreenPos.x + ((playheadTime - visibleStart) / duration) * rightWidth;
        float phYTop = rightPaneScreenPos.y;
        float phYBot = rightPaneScreenPos.y + panelSize.y - scrollbarHeight;

        
        ImGuiIO& io = ImGui::GetIO();
        ImVec2 mouse = io.MousePos;
        bool mouseInRightPane = mouse.x >= rightPaneScreenPos.x && mouse.x <= rightPaneScreenPos.x + rightWidth
                             && mouse.y >= phYTop && mouse.y <= phYBot;

        
        bool nearPlayhead = std::abs(mouse.x - phX) <= 8.0f && mouseInRightPane;

        if (!playheadDragging && nearPlayhead && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            playheadDragging = true;

        if (playheadDragging) {
            if (io.MouseDown[0]) {
                float newTime = visibleStart + ((mouse.x - rightPaneScreenPos.x) / rightWidth) * duration;
                if (newTime < 0.0f) newTime = 0.0f;
                if (newTime > projectDuration) newTime = projectDuration;
                playheadTime = newTime;
                
                phX = rightPaneScreenPos.x + ((playheadTime - visibleStart) / duration) * rightWidth;
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
            } else {
                playheadDragging = false;
            }
        } else if (nearPlayhead) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        }

        
        if (playheadTime >= visibleStart && playheadTime <= visibleEnd) {
            ImDrawList* overlayDraw = ImGui::GetForegroundDrawList();

            
            overlayDraw->PushClipRect(rightPaneScreenPos, ImVec2(rightPaneScreenPos.x + rightWidth, rightPaneScreenPos.y + panelSize.y), true);

            bool phHovered = nearPlayhead || playheadDragging;
            ImU32 phColor = phHovered ? IM_COL32(255, 80, 110, 255) : colPlayhead;

            overlayDraw->AddLine(ImVec2(phX, phYTop), ImVec2(phX, phYBot), phColor, phHovered ? 2.0f : 1.5f);
            overlayDraw->AddTriangleFilled(ImVec2(phX - 6, phYTop), ImVec2(phX + 6, phYTop), ImVec2(phX, phYTop + 10), phColor);

            overlayDraw->PopClipRect();
        }

        ImGui::EndChild(); 
    }
} 