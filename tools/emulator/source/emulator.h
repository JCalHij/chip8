#pragma once

#include <string>


typedef struct _Chip8 Chip8;


enum EmulatorMode
{
    Emulator_None = -1,
    Emulator_Paused = 0,
    Emulator_Ticking = 1,
    Emulator_Running = 2,
};

struct Emulator
{
    struct {
        unsigned int speed; // instructions per second
        EmulatorMode mode; // emulator running mode
    } configuration;

    struct {
        std::string rompath;
        double execution_accumulator; // Acummulates time until execution speed is matched, point at which an instruction is executed
        double timer_accumulator; // accumulates time until 16 ms, point at which it is reset and Chip8->delay is reduced
    } state;
    
    Chip8* ch8;
};

// Creates a new emulator with default values
Emulator* emulator_new();
// Destroys an emulator object
void emulator_delete(Emulator* em);

// Fresh start of the emulator
void emulator_reset(Emulator* em);

// Restarts the whole emulator, using the same ROM as before
void emulator_restart(Emulator* em);

// Loads ROM from given path into the CHIP-8
bool emulator_load_rom(Emulator* em, const std::string& rompath);

// The emulator executes its CHIP-8 as many times as required by delta_time and its speed
// delta_time in seconds
void emulator_tick(Emulator* em, double delta_time);