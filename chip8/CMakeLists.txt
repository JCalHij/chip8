project (chip8)

file(GLOB_RECURSE SOURCES "source/**.c")

add_library(chip8 STATIC ${SOURCES})
target_include_directories(chip8 PUBLIC "source/")
set_target_properties(chip8 PROPERTIES C_STANDARD 99)