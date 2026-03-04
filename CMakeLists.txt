cmake_minimum_required(VERSION 3.10)
project(MotionGraphicsEditor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(lib/imgui lib/glfw/include lib/glad/include)

set(IMGUI_SOURCES
    lib/imgui/imgui.cpp
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_widgets.cpp
    lib/imgui/imgui_tables.cpp
    lib/imgui/imgui_impl_glfw.cpp
    lib/imgui/imgui_impl_opengl3.cpp
)

set(APP_SOURCES
    src/main.cpp
    src/app_window.cpp
    src/top_panel.cpp
    src/bottom_panel.cpp
    src/core/project.cpp
    src/core/timeline.cpp
    src/core/renderer.cpp
    src/core/shape_renderer.cpp
    lib/glad/src/glad.c
)

add_executable(MotionGraphicsEditor ${APP_SOURCES} ${IMGUI_SOURCES})

target_link_libraries(MotionGraphicsEditor ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw/lib-vc2010-64/glfw3.lib opengl32.lib)