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

        //left side wit the menu
        ImGui::BeginChild("ChildT", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.5), ImGuiChildFlags_None);
        MenuPicker();
        ImGui::EndChild();

        //ImGui::SameLine(); uncomment if u want them to be side by side

        //right side with queues
        ImGui::BeginChild("ChildB", ImVec2(ImGui::GetContentRegionAvail().x, 0), ImGuiChildFlags_None);
        SchedQueues();
        ImGui::EndChild();


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
    }
    void SchedQueues() {
        ImGuiStyle style = ImGui::GetStyle();
        float child_w = (ImGui::GetContentRegionAvail().x - 4 * style.ItemSpacing.x) / 5;
        FCFS fcfs;
        auto fcfsQueue = fcfs.getQueue();
        ImGui::Columns(3, nullptr, false);
        for(int i = 0; i < 3; i++) {
            if(i > 0) ImGui::SameLine();
            ImGui::BeginGroup();
            const char* names[] = {"First Come First Serve", "Round Robin", "Priority Scheduling"};
            ImGui::TextUnformatted(names[i]);
            const ImGuiID child_id = ImGui::GetID((void*)(intptr_t)i);
            const bool child_is_visible = ImGui::BeginChild(child_id, ImVec2(0,200.0f), ImGuiChildFlags_Border);
            // @todo: get current processing order to highlight
            int currentOrder = 0;

            if(child_is_visible) {
                for(int order = (int)fcfsQueue.size() - 1; order >= 0; order--) {
                    if(order == currentOrder) {
                        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Order %d %s", order, fcfsQueue[order].item.name.c_str());
                    }
                    else {
                        ImGui::Text("Order %d %s", order, fcfsQueue[order].item.name.c_str());
                    }
                }
            }
            ImGui::EndChild();
            ImGui::EndGroup();
            ImGui::NextColumn();
        }
    }
}

