#include "top_panel.h"
#include "imgui.h"

namespace TopPanel {

    void Render(float height) {
        ImGui::BeginChild("TopPanel", ImVec2(0, height), true);
        ImGui::Text("Top Panel");
        ImGui::Text("Drag divider below");
        ImGui::EndChild();
    }

} // namespace TopPanel
