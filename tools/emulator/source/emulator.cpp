#include "emulator.h"

#include "input.h"
extern "C" {
    #include "chip8.h"
}

#include <SDL2/SDL_keycode.h>
#include <algorithm>
#include <string.h>


constexpr double Chip8DelayTimerPeriod = 1.0 / CHIP8_DELAY_TIMER_FREQ;


Emulator* emulator_new()
{
    Emulator* em = new Emulator();
    emulator_reset(em);

    return em;
}


void emulator_reset(Emulator* em)
{
    memset(em, 0, sizeof(Emulator));
    em->configuration.speed = 800;
    em->configuration.mode = Emulator_None;
    em->ch8 = chip8_new();
    chip8_init(em->ch8);
}


void emulator_restart(Emulator* em)
{
    std::string rompath{em->state.rompath};
    emulator_load_rom(em, rompath);
}


void emulator_delete(Emulator* em)
{
    chip8_delete(em->ch8);
    delete em;
}


bool emulator_load_rom(Emulator* em, const std::string& rompath)
{
    emulator_reset(em);
    em->state.rompath = rompath;
    bool load_ok = chip8_load_rom(em->ch8, rompath.c_str()) == 0;
    em->configuration.mode = load_ok? Emulator_Paused : Emulator_None;
    return load_ok;
}


// Runs the emulator in Running mode, executing multiple instructions
static void __tick_running(Emulator* em, double delta_time)
{
    double seconds_per_instruction = 1.0 / em->configuration.speed;
    em->state.execution_accumulator += delta_time;
    while (em->state.execution_accumulator >= seconds_per_instruction)
    {
        em->state.execution_accumulator -= seconds_per_instruction;
        chip8_execute(em->ch8);
    }

    em->state.timer_accumulator += delta_time;
    while (em->state.timer_accumulator >= Chip8DelayTimerPeriod)
    {
        em->state.timer_accumulator -= Chip8DelayTimerPeriod;
        em->ch8->delay_timer = std::max(0, em->ch8->delay_timer - 1);
    }
}


// Runs the emulator in Ticking mode, executing only one instruction
// at a time, and returning to Pause mode at the end
static void __tick_single(Emulator* em, double delta_time)
{
    double seconds_per_instruction = 1.0 / em->configuration.speed;

    chip8_execute(em->ch8);

    em->state.timer_accumulator += seconds_per_instruction;
    while (em->state.timer_accumulator >= Chip8DelayTimerPeriod)
    {
        em->state.timer_accumulator -= Chip8DelayTimerPeriod;
        em->ch8->delay_timer = std::max(0, em->ch8->delay_timer - 1);
    }

    em->configuration.mode = Emulator_Paused;
}


void emulator_tick(Emulator* em, double delta_time)
{
    em->ch8->keyboard[0x0] = input_is_key_held(SDLK_0)? 1:0;
    em->ch8->keyboard[0x1] = input_is_key_held(SDLK_1)? 1:0;
    em->ch8->keyboard[0x2] = input_is_key_held(SDLK_2)? 1:0;
    em->ch8->keyboard[0x3] = input_is_key_held(SDLK_3)? 1:0;
    em->ch8->keyboard[0x4] = input_is_key_held(SDLK_4)? 1:0;
    em->ch8->keyboard[0x5] = input_is_key_held(SDLK_5)? 1:0;
    em->ch8->keyboard[0x6] = input_is_key_held(SDLK_6)? 1:0;
    em->ch8->keyboard[0x7] = input_is_key_held(SDLK_7)? 1:0;
    em->ch8->keyboard[0x8] = input_is_key_held(SDLK_8)? 1:0;
    em->ch8->keyboard[0x9] = input_is_key_held(SDLK_9)? 1:0;
    em->ch8->keyboard[0xA] = input_is_key_held(SDLK_a)? 1:0;
    em->ch8->keyboard[0xB] = input_is_key_held(SDLK_b)? 1:0;
    em->ch8->keyboard[0xC] = input_is_key_held(SDLK_c)? 1:0;
    em->ch8->keyboard[0xD] = input_is_key_held(SDLK_d)? 1:0;
    em->ch8->keyboard[0xE] = input_is_key_held(SDLK_e)? 1:0;
    em->ch8->keyboard[0xF] = input_is_key_held(SDLK_f)? 1:0;
    
    switch(em->configuration.mode)
    {
        case Emulator_Running: { __tick_running(em, delta_time); break; }
        case Emulator_Ticking: { __tick_single(em, delta_time); break; }
        case Emulator_Paused:
        default: { break; }
    }
}