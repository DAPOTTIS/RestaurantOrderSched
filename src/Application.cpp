#include "Application.h"

#include <stdexcept>
#include <vector> 
#include <algorithm> 
#include <optional>  // For std::optional

#include "imgui.h"
#include "Menu.h"
#include "FCFS.h"   // Ensure .h files declare getCurrentlyProcessingOrder()
#include "Order.h"
#include "RR.h"     // Ensure .h files declare getCurrentlyProcessingOrder()
#include "PQ.h"     // Ensure .h files declare getCurrentlyProcessingOrder()


namespace App {
    static FCFS fcfs_scheduler; 
    static RR rr_scheduler(6); 
    static PriorityScheduler priority_scheduler_instance; 

    static bool show_completed_orders_window = false; // Flag to control visibility of the completed orders window

    // Forward declaration for the completed orders window function
    void RenderCompletedOrdersWindow(); 

    void RenderUI() {
        fcfs_scheduler.start();
        rr_scheduler.start();
        priority_scheduler_instance.start();

        bool collapse = true;
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
        ImGui::Begin("Restaurant System", &collapse, flags);

        ImGui::BeginChild("ChildT", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.5), ImGuiChildFlags_None);
        MenuPicker();
        ImGui::EndChild();

        ImGui::BeginChild("ChildB", ImVec2(ImGui::GetContentRegionAvail().x, 0), ImGuiChildFlags_None);
        SchedQueues();
        ImGui::EndChild();

        if (ImGui::Button("Show/Hide Completed Orders")) {
            show_completed_orders_window = !show_completed_orders_window;
        }

        ImGui::End();

        // Render the completed orders window if the flag is set
        if (show_completed_orders_window) {
            RenderCompletedOrdersWindow();
        }
    }

    void MenuPicker() {
        // ... (MenuPicker remains largely the same, adding orders to schedulers) ...
        Menu _menu("egyptian");
        Menu egyptianMenu = _menu.createEgyptianMenu(); 
        const auto& menuItems = egyptianMenu.getItems();
        
        static ImGuiTableFlags flags =
                    ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
                    | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
                    | ImGuiTableFlags_ScrollY;
        if(ImGui::BeginTable("menu_table", 5, flags, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 15), 0.0f)) {
            // ... table setup ...
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_DefaultSort, 0.0f);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 0.0f);
            ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 0.0f);
            ImGui::TableSetupColumn("Price", ImGuiTableColumnFlags_WidthFixed, 0.0f);
            ImGui::TableSetupColumn("Burst Time", ImGuiTableColumnFlags_WidthStretch, 0.0f);
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableHeadersRow();

            for(const auto &item : menuItems) {
                ImGui::PushID(item.id);
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%d", item.id);
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(item.name.c_str());
                ImGui::TableNextColumn();
                if (ImGui::SmallButton("Order"))
                {
                    Order newOrder(item); 
                    fcfs_scheduler.addOrder(newOrder); 
                    rr_scheduler.addOrder(newOrder);
                    priority_scheduler_instance.addOrder(newOrder); 
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

    // Modified renderQueue to take currently processing item and upcoming queue
    template<typename Container>
    void renderQueue(const std::optional<Order>& currentOrderOpt, const Container& upcomingQueue) {
        if (currentOrderOpt) {
            const Order& currentOrder = *currentOrderOpt;
            // Highlight the currently processing order
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); // Yellow
            ImGui::TextWrapped("Current: ID %d: %s (Prep: %d, Rem: %d)", 
                         currentOrder.getOrderId(), 
                         currentOrder.getItemName().c_str(), 
                         currentOrder.getPrepTime(),
                         currentOrder.getRemainingTime()); // Assuming getRemainingTime exists
            ImGui::PopStyleColor();
            ImGui::Separator();
        } else {
            ImGui::TextUnformatted("Current: --- Idle ---");
            ImGui::Separator();
        }

        ImGui::TextUnformatted("Upcoming Orders:");
        if (upcomingQueue.empty() && !currentOrderOpt) { // Show idle only if no current and no upcoming
             // Handled by "Idle" above if no currentOrderOpt
        } else if (upcomingQueue.empty() && currentOrderOpt) {
            ImGui::TextUnformatted("(None)");
        }


        int upcomingIndex = 0;
        for(const auto& order : upcomingQueue) {
            ImGui::Text("Upcoming %d: ID %d: %s (Prep: %d, Rem: %d)", 
                      upcomingIndex++, 
                      order.getOrderId(), 
                      order.getItemName().c_str(), 
                      order.getPrepTime(),
                      order.getRemainingTime());
        }
    }

    void SchedQueues() {
        // Get "currently processing" orders
        const auto fcfsCurrent = FCFS::getCurrentlyProcessingOrder();
        const auto rrCurrent = RR::getCurrentlyProcessingOrder();
        const auto pqCurrent = PriorityScheduler::getCurrentlyProcessingOrder();

        // Get "upcoming" orders queues
        const auto fcfsUpcomingQueue = FCFS::getQueue(); 
        const auto rrUpcomingQueue = RR::getQueue();     
        
        const auto highPriorityUpcoming = priority_scheduler_instance.getHighPriorityQueueCopy();
        const auto lowPriorityUpcoming = priority_scheduler_instance.getLowPriorityQueueCopy();
        
        std::vector<Order> combinedPriorityUpcomingQueue;
        combinedPriorityUpcomingQueue.insert(combinedPriorityUpcomingQueue.end(), highPriorityUpcoming.begin(), highPriorityUpcoming.end());
        combinedPriorityUpcomingQueue.insert(combinedPriorityUpcomingQueue.end(), lowPriorityUpcoming.begin(), lowPriorityUpcoming.end());

        ImGui::Columns(3, nullptr, false);
        for(int i = 0; i < 3; i++) {
            ImGui::BeginGroup();
            const char* names[] = {"First Come First Serve", "Round Robin", "Priority Scheduling"};
            ImGui::TextUnformatted(names[i]);
            const ImGuiID child_id = ImGui::GetID((void*)(intptr_t)i);
            const bool child_is_visible = ImGui::BeginChild(child_id, ImVec2(ImGui::GetColumnWidth() * 0.95f, 200.0f), ImGuiChildFlags_Border);
            
            if(child_is_visible) {
                switch(i) {
                    case 0 : renderQueue(fcfsCurrent, fcfsUpcomingQueue); break;
                    case 1 : renderQueue(rrCurrent, rrUpcomingQueue); break;
                    case 2 : renderQueue(pqCurrent, combinedPriorityUpcomingQueue); break; 
                    default: 
                        throw std::runtime_error("Invalid queue type index in SchedQueues");
                }
            }
            ImGui::EndChild();
            ImGui::EndGroup();
            ImGui::NextColumn();
        }
        ImGui::Columns(1); 
    }

    // Helper function to render a list of completed orders for a scheduler
    void RenderCompletedOrdersList(const char* scheduler_name, const std::vector<Order>& completed_orders) {
        if (ImGui::TreeNode(scheduler_name)) {
            if (completed_orders.empty()) {
                ImGui::TextUnformatted("No orders completed yet.");
            } else {
                ImGuiListClipper clipper;
                clipper.Begin(completed_orders.size());
                while (clipper.Step()) {
                    for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
                        const auto& order = completed_orders[i];
                        ImGui::Text("ID: %d, Item: %s, Prep Time: %d",
                                    order.getOrderId(),
                                    order.getItemName().c_str(),
                                    order.getPrepTime());
                    }
                }
                clipper.End();
            }
            ImGui::TreePop();
        }
    }

    // Function to render the "Completed Orders" window
    void RenderCompletedOrdersWindow() {
        if (!show_completed_orders_window) {
            return;
        }

        ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Completed Orders", &show_completed_orders_window)) {
            const auto fcfsCompleted = FCFS::getCompletedOrders();
            RenderCompletedOrdersList("FCFS Completed Orders", fcfsCompleted);

            ImGui::Separator();

            const auto rrCompleted = RR::getCompletedOrders();
            RenderCompletedOrdersList("Round Robin Completed Orders", rrCompleted);

            ImGui::Separator();

            const auto pqCompleted = PriorityScheduler::getCompletedOrders();
            RenderCompletedOrdersList("Priority Queue Completed Orders", pqCompleted);
        }
        ImGui::End();
    }
}
