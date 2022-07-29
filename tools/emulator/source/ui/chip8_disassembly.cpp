#include "ui.h"

extern "C" {
    #include "chip8.h"
}

#include "imgui.h"


void ui_chip8_disassembly(Chip8* ch8)
{
    if (!ImGui::Begin("Disassembly"))
    {
        ImGui::End();
        return;
    }

    uint16_t pc = CHIP8_PROGRAM_START_LOCATION;
    char disassembly_inst[256];
    while (pc < CHIP8_PROGRAM_START_LOCATION + ch8->rom_size)
    {
        chip8_disassemble_at(ch8, pc, disassembly_inst);
        ImGui::TextColored(ch8->pc == pc? ImVec4{1.0f, 0.0f, 0.0f, 1.0f} : ImVec4{1.0f,1.0f,1.0f,1.0f}, disassembly_inst);
        pc += 2;
    }

    ImGui::End();
}