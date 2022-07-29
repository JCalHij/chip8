#include "ui.h"

#include "emulator.h"

#include "imgui.h"


static unsigned int speed_min = 1;
static unsigned int speed_max = 1000;

static int rompath_idx; // Here we store our selection data as an index.
static std::string rom_paths[] = {
    "data/chip8-roms/demos/Maze (alt) [David Winter, 199x].ch8",
    "data/chip8-roms/demos/Maze [David Winter, 199x].ch8",
    "data/chip8-roms/demos/Particle Demo [zeroZshadow, 2008].ch8",
    "data/chip8-roms/demos/Sierpinski [Sergey Naydenov, 2010].ch8",
    "data/chip8-roms/demos/Stars [Sergey Naydenov, 2010].ch8",
    "data/chip8-roms/demos/Trip8 Demo (2008) [Revival Studios].ch8",
    "data/chip8-roms/demos/Zero Demo [zeroZshadow, 2007].ch8",
    "data/chip8-roms/games/Breakout (Brix hack) [David Winter, 1997].ch8",
    "data/chip8-roms/games/Cave.ch8",
};


void ui_emulation_controls(Emulator* emulator)
{
    if (!ImGui::Begin("Controller"))
    {
        ImGui::End();
        return;
    }

    // Control buttons
    if(ImGui::Button("Run") && emulator->configuration.mode != Emulator_None)
    {
        emulator->configuration.mode = Emulator_Running;
    }
    ImGui::SameLine();
    if(ImGui::Button("Tick") && emulator->configuration.mode != Emulator_None)
    {
        emulator->configuration.mode = Emulator_Ticking;
    }
    ImGui::SameLine();
    if(ImGui::Button("Pause") && emulator->configuration.mode != Emulator_None)
    {
        emulator->configuration.mode = Emulator_Paused;
    }
    ImGui::SameLine();
    if(ImGui::Button("Restart") && emulator->configuration.mode != Emulator_None)
    {
        emulator_restart(emulator);
    }
    ImGui::SameLine();
    if(ImGui::Button("Reset"))
    {
        emulator_reset(emulator);
    }

    ImGui::SliderScalar("Speed [Hz]", ImGuiDataType_U32, &emulator->configuration.speed, &speed_min, &speed_max, "%u");

    ImGui::LabelText("Exec. Acc. [ms]", "%.04f", &emulator->state.execution_accumulator);
    ImGui::LabelText("Timer Acc. [ms]", "%.04f", &emulator->state.timer_accumulator);

    if(ImGui::Button("Load ROM"))
    {
        emulator_load_rom(emulator, rom_paths[rompath_idx]);
    }
    ImGui::Text("ROM Selection");
    if (ImGui::BeginListBox("##ROM Selection", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
    {
        for (int n = 0; n < IM_ARRAYSIZE(rom_paths); n++)
        {
            const bool is_selected = (rompath_idx == n);
            if (ImGui::Selectable(rom_paths[n].c_str(), is_selected))
                rompath_idx = n;

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }

    ImGui::End();
}