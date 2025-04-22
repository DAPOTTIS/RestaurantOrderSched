cmake_minimum_required(VERSION 3.10)
project(RestaurantOrderSched)

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
        src/Menu.cpp
        src/Menu.h
        src/MenuItem.h
        src/CLI.cpp
        src/CLI.h
        src/States.cpp
        src/States.h
        src/GUI.cpp
        src/GUI.h
        src/FCFS.cpp
        src/FCFS.h
        src/RR.cpp
        src/RR.h
)

# Include directories for headers (both lib and src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)


# Find Vulkan
#set(Vulkan_INCLUDE_DIR "C:/VulkanSDK/1.4.309.0/Include")
#set(Vulkan_LIBRARY "C:/VulkanSDK/1.4.309.0/Lib/vulkan-1.lib")
find_package(Vulkan REQUIRED)

if(NOT GLFW_DIR)
    set(GLFW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glfw) # Set this to point to an up-to-date GLFW repo
endif()
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)
add_subdirectory(${GLFW_DIR} binary_dir EXCLUDE_FROM_ALL)
include_directories(${GLFW_DIR}/include)



# Create the executable combining your project and ImGui sources
add_executable(${PROJECT_NAME} ${SRC_SOURCES} ${IMGUI_SOURCES})

# Link Vulkan and GLFW libraries to your target
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan glfw)

# Optionally, you may want to add additional definitions or compile options here.
