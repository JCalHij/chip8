#include "ui.h"

#include "chip8.h"

#include "imgui.h"


void ui_chip8_vram(Chip8* ch8)
{
    if (!ImGui::Begin("VRAM"))
    {
        ImGui::End();
        return;
    }

    for (int y = 0; y < CHIP8_DISPLAY_HEIGHT; ++y)
    {
        for (int x = 0; x < CHIP8_DISPLAY_WIDTH; ++x)
        {
            ImGui::TextColored(ch8->VRAM[VRAM_AT(x, y)] != 0? ImVec4{1.0, 1.0, 1.0, 1.0} : ImVec4{0.2, 0.2, 0.2, 1.0}, "X");
            if (x != CHIP8_DISPLAY_WIDTH-1) ImGui::SameLine();
        }
    }

    ImGui::End();
}