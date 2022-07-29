#include "ui.h"

#include "chip8.h"

#include "imgui.h"
#include "imgui_memory_editor/imgui_memory_editor.h"

static MemoryEditor memory_editor;

void ui_chip8_ram(Chip8* ch8)
{
    memory_editor.DrawWindow("RAM Viewer", ch8->memory, CHIP8_MEMORY_SIZE);
}