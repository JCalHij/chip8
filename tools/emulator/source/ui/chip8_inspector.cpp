#include "ui.h"

extern "C" {
    #include "chip8.h"
}

#include "imgui.h"

#include <iostream>

const ImVec4 White{1.0f, 1.0f, 1.0f, 1.0f};
const ImVec4 Faded{0.3f, 0.3f, 0.3f, 1.0f};


void ui_chip8_inspector(Chip8* ch8)
{
    if (!ImGui::Begin("Inspector"))
    {
        ImGui::End();
        return;
    }

    ImGui::Text("State");

    ImGui::TextColored(White, "I");
    ImGui::SameLine();
    ImGui::TextColored(White, "0x%04X", ch8->I);

    ImGui::TextColored(White, "PC");
    ImGui::SameLine();
    ImGui::TextColored(White, "0x%04X", ch8->pc);

    ImGui::TextColored(White, "Delay Timer");
    ImGui::SameLine();
    ImGui::TextColored(White, "%d", ch8->delay_timer);

    ImGui::TextColored(White, "Sound Timer");
    ImGui::SameLine();
    ImGui::TextColored(White, "%d", ch8->sound_timer);

    ImGui::Separator();

    ImGui::Text("Registers");
    for(int i = 0; i < CHIP8_NUM_REGISTERS; ++i)
    {
        ImGui::TextColored(White, "Vx%01X", i);
        ImGui::SameLine();
        ImGui::TextColored(White, "0x%02X", ch8->v[i]);
        ImGui::SameLine();
        ++i;
        ImGui::TextColored(White, "Vx%01X", i);
        ImGui::SameLine();
        ImGui::TextColored(White, "0x%02X", ch8->v[i]);
        ImGui::SameLine();
        ++i;
        ImGui::TextColored(White, "Vx%01X", i);
        ImGui::SameLine();
        ImGui::TextColored(White, "0x%02X", ch8->v[i]);
        ImGui::SameLine();
        ++i;
        ImGui::TextColored(White, "Vx%01X", i);
        ImGui::SameLine();
        ImGui::TextColored(White, "0x%02X", ch8->v[i]);
    }

    ImGui::Separator();

    ImGui::Text("Stack");

    for (int i = 0; i < CHIP8_STACK_SIZE; ++i)
    {
        ImGui::TextColored(ch8->sp>i? White : Faded, "%02d", i);
        ImGui::SameLine();
        ImGui::TextColored(ch8->sp>i? White : Faded,  "0x%04X", ch8->stack[i]);
        ImGui::SameLine();
        ++i;

        ImGui::TextColored(ch8->sp>i? White : Faded, "%02d", i);
        ImGui::SameLine();
        ImGui::TextColored(ch8->sp>i? White : Faded, "0x%04X", ch8->stack[i]);
    }


    ImGui::End();
}