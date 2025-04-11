#include "Application.h"
#include "imgui.h"
#include "Menu.h"
#include "FCFS.h"
#include "Order.h"


namespace App {
    void RenderUI() {
        bool collapse = true;
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
        ImGui::Begin("Restaurant System", &collapse, flags);
        MenuPicker();
        ImGui::End();
        ImGui::ShowDemoWindow();
    }

    void MenuPicker() {
        Menu _menu("egyptian");
        Menu egyptianMenu = _menu.createEgyptianMenu();
        const auto& menu = egyptianMenu.getItems();
        static FCFS scheduler; // Made static to extend lifetime for the processing thread
        scheduler.start();
        static ImGuiTableFlags flags =
                    ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
                    | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
                    | ImGuiTableFlags_ScrollY;
        if(ImGui::BeginTable("menu_table", 5, flags, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 15), 0.0f)) {
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_DefaultSort, 0.0f);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 0.0f);
            ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 0.0f);
            ImGui::TableSetupColumn("Price", ImGuiTableColumnFlags_WidthFixed, 0.0f);
            ImGui::TableSetupColumn("Burst Time", ImGuiTableColumnFlags_WidthStretch, 0.0f);
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableHeadersRow();

            for(const auto &item : menu) {
                ImGui::PushID(item.id);
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%d", item.id);
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(item.name.c_str());
                ImGui::TableNextColumn();
                if (ImGui::SmallButton("Order"))
                {
                    scheduler.addOrder(Order(item)); // Add the order to the scheduler
                }
                ImGui::TableNextColumn();
                ImGui::Text("%d", item.price);
                ImGui::TableNextColumn();
                ImGui::Text("%d", item.burstTime);
                ImGui::PopID();
            }
        ImGui::EndTable();
        }
        if(ImGui::Button("Exit")){
            scheduler.stop(); // Stop processing orders
            ImGui::CloseCurrentPopup(); // Close the menu
            exit(0); // Exit the application
        }
    }
}

