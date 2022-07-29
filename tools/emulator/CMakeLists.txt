project(Emulator)

file(GLOB_RECURSE SOURCES "source/**.cpp")

add_executable(Emulator ${SOURCES})
target_include_directories(Emulator PRIVATE "source/")
target_link_libraries(Emulator PRIVATE SDL2 dl GL chip8 ImGui ImGui_MemoryEditor)