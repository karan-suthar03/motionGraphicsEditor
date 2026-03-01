cmake_minimum_required(VERSION 3.10)
project(MotionGraphicsEditor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(lib/imgui lib/glfw/include)

set(IMGUI_SOURCES
    lib/imgui/imgui.cpp
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_widgets.cpp
    lib/imgui/imgui_tables.cpp
    lib/imgui/imgui_impl_glfw.cpp
    lib/imgui/imgui_impl_opengl3.cpp
)

add_executable(MotionGraphicsEditor src/main.cpp ${IMGUI_SOURCES})

target_link_libraries(MotionGraphicsEditor ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw/lib-vc2010-64/glfw3.lib opengl32.lib)