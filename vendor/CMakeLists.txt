file(GLOB IMGUI_SOURCES "imgui/*.cpp")
add_library(ImGui STATIC ${IMGUI_SOURCES})
target_include_directories(ImGui PUBLIC "imgui/")

add_library(ImGui_MemoryEditor INTERFACE)
target_include_directories(ImGui_MemoryEditor INTERFACE "imgui_club/")