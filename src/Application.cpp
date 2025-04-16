#include "Application.h"

#include <stdexcept>

#include "imgui.h"
#include "Menu.h"
#include "FCFS.h"
#include "Order.h"
#include "RR.h"


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
        static FCFS fcfs; // Made static to extend lifetime for the processing thread
        static RR rr(6);
        fcfs.start();
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
                    fcfs.addOrder(Order(item)); // Add the order to the scheduler
                    rr.addOrder(Order(item));
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

    template<typename Container>
        void renderQueue(const Container& queue) {
        int currentOrder = 0;
        for(int order = static_cast<int>(queue.size()) - 1; order >= 0; --order) {
            if(order == currentOrder) {
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "Order %d %s", order, queue[order].item.name.c_str());
            }
            else {
                ImGui::Text("Order %d %s", order, queue[order].item.name.c_str());
            }
        }
    }

    void SchedQueues() {
        const auto fcfsQueue = FCFS::getQueue();
        const auto rrQueue = RR::getQueue();

        ImGui::Columns(3, nullptr, false);
        for(int i = 0; i < 3; i++) {
            if(i > 0) ImGui::SameLine();
            ImGui::BeginGroup();
            const char* names[] = {"First Come First Serve", "Round Robin", "Priority Scheduling"};
            ImGui::TextUnformatted(names[i]);
            const ImGuiID child_id = ImGui::GetID((void*)(intptr_t)i);
            const bool child_is_visible = ImGui::BeginChild(child_id, ImVec2(0,200.0f), ImGuiChildFlags_Border);
            // @todo: get current processing order to highlight

            if(child_is_visible) {
                switch(i) {
                    case 0 : renderQueue(fcfsQueue); break;
                    case 1 : renderQueue(rrQueue); break;
                    case 2 : renderQueue(fcfsQueue); break;
                    default: throw std::runtime_error("Invalid queue type");
                }
            }
            ImGui::EndChild();
            ImGui::EndGroup();
            ImGui::NextColumn();
        }
    }
}

