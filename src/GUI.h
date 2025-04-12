#ifndef GUI_H
#define GUI_H

#include <functional>

#include "imgui.h"
#include "imgui_impl_vulkan.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN


class GUI {
    // Data


    static void glfw_error_callback(int error, const char *description);

    static void check_vk_result(VkResult err);

    static bool IsExtensionAvailable(const ImVector<VkExtensionProperties> &properties, const char *extension);

    static void SetupVulkan(ImVector<const char *> instance_extensions);

    // All the ImGui_ImplVulkanH_XXX structures/functions are optional helpers used by the demo.
    // Your real engine/app may not use them.
    void SetupVulkanWindow(ImGui_ImplVulkanH_Window *wd, VkSurfaceKHR surface, int width, int height);

    void CleanupVulkan();

    void CleanupVulkanWindow();

    void FrameRender(ImGui_ImplVulkanH_Window *wd, ImDrawData *draw_data);

    void FramePresent(ImGui_ImplVulkanH_Window *wd);

    using UIRenderFunc = std::function<void()>;
    void RenderFrame(ImGui_ImplVulkanH_Window *wd);

    void setTheme();
public:

    int main_loop();
};


#endif //GUI_H
