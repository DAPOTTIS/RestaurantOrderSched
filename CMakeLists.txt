cmake_minimum_required(VERSION 3.10)
project(ImGuiVulkanExample)

# Set C++ standard (adjust as needed)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define source files for ImGui (located in lib)
set(IMGUI_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui_demo.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui_draw.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui_tables.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui_widgets.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui_impl_glfw.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui_impl_vulkan.cpp
)

# Define source files for your project (located in src)
set(SRC_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Order.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Scheduler.cpp
        src/Application.cpp
        src/Application.h
)

# Include directories for headers (both lib and src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)

# Find Vulkan
find_package(Vulkan REQUIRED)

# Find GLFW (assuming you have it installed; adjust if you’re using a custom build)
set(glfw3_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/cmake/glfw3")
find_package(glfw3 REQUIRED)

# Create the executable combining your project and ImGui sources
add_executable(${PROJECT_NAME} ${SRC_SOURCES} ${IMGUI_SOURCES})

# Link Vulkan and GLFW libraries to your target
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan glfw)

# Optionally, you may want to add additional definitions or compile options here.
