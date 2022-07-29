#pragma once


typedef struct _Chip8 Chip8;
struct Emulator;


void ui_emulation_controls(Emulator* emulator);
void ui_chip8_ram(Chip8* ch8);
void ui_chip8_inspector(Chip8* ch8);
void ui_chip8_disassembly(Chip8* ch8);
void ui_chip8_vram(Chip8* ch8);