cmake_minimum_required(VERSION 3.10)

project(RestaurantOrderScheduler VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(LIB_DIR "${PROJECT_SOURCE_DIR}/lib")

# gather all source files from the src directory
file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")

add_executable(RestaurantOrderScheduler ${SOURCES}
        src/Order.cpp
        src/Order.h
        src/Scheduler.cpp
        src/Scheduler.h
        src/Menu.cpp
        src/Menu.h
        src/MenuItem.h)

# include dirs for third party includes and libs
target_include_directories(RestaurantOrderScheduler
    PRIVATE ${INCLUDE_DIR}
    PRIVATE ${LIB_DIR}
)

# for when we do ImGui:
# add_subdirectory(${LIB_DIR}/imgui)
# target_link_libraries(RestaurantOrderScheduler PRIVATE imgui)

# Enable testing and add the tests directory if it exists
#enable_testing()
#if(EXISTS "${PROJECT_SOURCE_DIR}/tests/CMakeLists.txt")
#    add_subdirectory(tests)
#endif()

