#include "Application.h"
#include "imgui.h"

namespace App {
    void RenderUI() {
        ImGui::Begin("Restaurant System");
        ImGui::Button("Place order");
        static float value = 0.0f;
        ImGui::DragFloat("Time", &value);
        ImGui::End();

        ImGui::ShowDemoWindow();
    }

}