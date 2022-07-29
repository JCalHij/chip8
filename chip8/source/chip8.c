#include "chip8.h"

#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>



// Font data (0-F)
// Source: https://github.com/mattmikolay/chip-8/wiki/Mastering-CHIP%E2%80%908
static const uint8_t font_data[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,
    0x20, 0x60, 0x20, 0x20, 0x70,
    0xF0, 0x10, 0xF0, 0x80, 0xF0,
    0xF0, 0x10, 0xF0, 0x10, 0xF0,
    0x90, 0x90, 0xF0, 0x10, 0x10,
    0xF0, 0x80, 0xF0, 0x10, 0xF0,
    0xF0, 0x80, 0xF0, 0x90, 0xF0,
    0xF0, 0x10, 0x20, 0x40, 0x40,
    0xF0, 0x90, 0xF0, 0x90, 0xF0,
    0xF0, 0x90, 0xF0, 0x10, 0xF0,
    0xF0, 0x90, 0xF0, 0x90, 0x90,
    0xE0, 0x90, 0xE0, 0x90, 0xE0,
    0xF0, 0x80, 0x80, 0x80, 0xF0,
    0xE0, 0x90, 0x90, 0x90, 0xE0,
    0xF0, 0x80, 0xF0, 0x80, 0xF0,
    0xF0, 0x80, 0xF0, 0x80, 0x80,
};

// 8-bit low and high nibble
// Example
// x -> 0101 0111
// HIGH_NIBBLE(x) -> 0000 0101
// LOW_NIBBLE(x) -> 0000 0111
#define LOW_NIBBLE(x) ( (x) & 0x0F )
#define HIGH_NIBBLE(x) ( ((x) >> 4) & 0x0F )

// Get bit b for x (results in either 0x0 or 0x1)
#define BIT(x, b) ( ((x) >> (b)) & 0x1 )

// Make a uint16_t out of two uint8_t
#define U16(a, b) ( ((a)<<8) | (b) )


Chip8* chip8_new()
{
    Chip8* ch8 = malloc(sizeof(Chip8));
    return ch8;
}


void chip8_delete(Chip8* ch8)
{
    free(ch8);
}


void chip8_init(Chip8* chip8)
{
    // Initialize memory
    memset(chip8, 0x0, sizeof(Chip8));
    memcpy(&chip8->memory[0x0], &font_data[0], sizeof(font_data)/sizeof(font_data[0]));
    chip8->pc = CHIP8_PROGRAM_START_LOCATION;
}


int chip8_load_rom(Chip8* chip8, const char* rom_path)
{
    FILE* rom = fopen(rom_path, "rb");
    if (!rom)
    {
        printf("Rom file could not be loaded.\n");
        return -1;
    }

    fseek(rom, 0, SEEK_END);
    chip8->rom_size = ftell(rom);
    fseek(rom, 0, SEEK_SET);

    for (size_t i = 0; i < chip8->rom_size; ++i)
    {
        fread(&chip8->memory[CHIP8_PROGRAM_START_LOCATION + i], sizeof(uint8_t), 1, rom);
    }

    fclose(rom);  

    return 0;  
}


void chip8_disassemble_at(Chip8* chip8, uint16_t at, char* dst)
{
    uint8_t* code = &chip8->memory[at];

    switch(HIGH_NIBBLE(code[0]))
    {
        case 0x0: {
            switch(U16(code[0], code[1]))
            {
                case 0x00E0: { sprintf(dst, "$%04X | 0x%02X%02X -> disp_clear()\n", at, code[0], code[1]); break; }
                case 0x00EE: { sprintf(dst, "$%04X | 0x%02X%02X -> return\n", at, code[0], code[1]); break; }
                default: { sprintf(dst, "$%04X | 0x%02X%02X -> call subroutine at 0x%03X\n", at, code[0], code[1], U16(code[0], code[1]) & 0x0FFF); break; }
            }
            break;
        }
        case 0x1: { sprintf(dst, "$%04X | 0x%02X%02X -> goto $%03X\n", at, code[0], code[1], U16(code[0], code[1]) & 0x0FFF); break; }
        case 0x2: { sprintf(dst, "$%04X | 0x%02X%02X -> *(0x%03X)()\n", at, code[0], code[1], U16(code[0], code[1]) & 0x0FFF); break; }
        case 0x3: { sprintf(dst, "$%04X | 0x%02X%02X -> skip if (V%1X == 0x%02X)\n", at, code[0], code[1], LOW_NIBBLE(code[0]), code[1]); break; }
        case 0x4: { sprintf(dst, "$%04X | 0x%02X%02X -> skip if (V%1X != 0x%02X)\n", at, code[0], code[1], LOW_NIBBLE(code[0]), code[1]); break; }
        case 0x5: { if(LOW_NIBBLE(code[1]) == 0) { sprintf(dst, "$%04X | 0x%02X%02X -> skip if (V%1X == V%1X)\n", at, code[0], code[1], LOW_NIBBLE(code[0]), HIGH_NIBBLE(code[1])); break; } }
        case 0x6: { sprintf(dst, "$%04X | 0x%02X%02X -> V%1X = 0x%02X\n", at, code[0], code[1], LOW_NIBBLE(code[0]), code[1]); break; }
        case 0x7: { sprintf(dst, "$%04X | 0x%02X%02X -> V%1X += 0x%02X\n", at, code[0], code[1], LOW_NIBBLE(code[0]), code[1]); break; }
        case 0x8: {
            switch(LOW_NIBBLE(code[1]))
            {
                case 0x0: { sprintf(dst, "$%04X | 0x%02X%02X -> V%1X = V%1X\n", at, code[0], code[1], LOW_NIBBLE(code[0]), HIGH_NIBBLE(code[1])); break; }
                case 0x1: { sprintf(dst, "$%04X | 0x%02X%02X -> V%1X |= V%1X\n", at, code[0], code[1], LOW_NIBBLE(code[0]), HIGH_NIBBLE(code[1])); break; }
                case 0x2: { sprintf(dst, "$%04X | 0x%02X%02X -> V%1X &= V%1X\n", at, code[0], code[1], LOW_NIBBLE(code[0]), HIGH_NIBBLE(code[1])); break; }
                case 0x3: { sprintf(dst, "$%04X | 0x%02X%02X -> V%1X ^= V%1X\n", at, code[0], code[1], LOW_NIBBLE(code[0]), HIGH_NIBBLE(code[1])); break; }
                case 0x4: { sprintf(dst, "$%04X | 0x%02X%02X -> V%1X += V%1X\n", at, code[0], code[1], LOW_NIBBLE(code[0]), HIGH_NIBBLE(code[1])); break; }
                case 0x5: { sprintf(dst, "$%04X | 0x%02X%02X -> V%1X -= V%1X\n", at, code[0], code[1], LOW_NIBBLE(code[0]), HIGH_NIBBLE(code[1])); break; }
                case 0x6: { sprintf(dst, "$%04X | 0x%02X%02X -> V%1X >>= 1\n", at, code[0], code[1], LOW_NIBBLE(code[0])); break; }
                case 0x7: { sprintf(dst, "$%04X | 0x%02X%02X -> V%1X = V%1X - V%1X\n", at, code[0], code[1], LOW_NIBBLE(code[0]), HIGH_NIBBLE(code[1]), LOW_NIBBLE(code[0])); break; }
                case 0xE: { sprintf(dst, "$%04X | 0x%02X%02X -> V%1X <<= 1\n", at, code[0], code[1], LOW_NIBBLE(code[0])); break; }
            }
            break; 
        }
        case 0x9: { if(LOW_NIBBLE(code[1]) == 0) { sprintf(dst, "$%04X | 0x%02X%02X -> skip if (V%1X != V%1X)\n", at, code[0], code[1], LOW_NIBBLE(code[0]), HIGH_NIBBLE(code[1])); break; } }
        case 0xA: { sprintf(dst, "$%04X | 0x%02X%02X -> I = 0x%03X\n", at, code[0], code[1], U16(code[0], code[1]) & 0x0FFF); break; }
        case 0xB: { sprintf(dst, "$%04X | 0x%02X%02X -> PC = V0 + 0x%03X\n", at, code[0], code[1], U16(code[0], code[1]) & 0x0FFF); break; }
        case 0xC: { sprintf(dst, "$%04X | 0x%02X%02X -> V%1X = rand() & 0x%02X\n", at, code[0], code[1], LOW_NIBBLE(code[0]), code[1]); break; }
        case 0xD: { sprintf(dst, "$%04X | 0x%02X%02X -> draw(V%1X, V%1X, 0x%1X)\n", at, code[0], code[1], LOW_NIBBLE(code[0]), HIGH_NIBBLE(code[1]), LOW_NIBBLE(code[1])); break; }
        case 0xE: {
            switch(code[1])
            {
                case 0x9E: { sprintf(dst, "$%04X | 0x%02X%02X -> skip if (is_key_pressed(V%1X))\n", at, code[0], code[1], LOW_NIBBLE(code[0])); break; }
                case 0xA1: { sprintf(dst, "$%04X | 0x%02X%02X -> skip if (!is_key_pressed(V%1X))\n", at, code[0], code[1], LOW_NIBBLE(code[0])); break; }
            }
            break; 
        }
        case 0xF: { 
            switch(code[1])
            {
                case 0x07: { sprintf(dst, "$%04X | 0x%02X%02X -> V%1X = delay_timer\n", at, code[0], code[1], LOW_NIBBLE(code[0])); break; }
                case 0x0A: { sprintf(dst, "$%04X | 0x%02X%02X -> V%1X = key_pressed\n", at, code[0], code[1], LOW_NIBBLE(code[0])); break; }
                case 0x15: { sprintf(dst, "$%04X | 0x%02X%02X -> delay_timer = V%1X\n", at, code[0], code[1], LOW_NIBBLE(code[0])); break; }
                case 0x18: { sprintf(dst, "$%04X | 0x%02X%02X -> sound_timer = V%1X\n", at, code[0], code[1], LOW_NIBBLE(code[0])); break; }
                case 0x1E: { sprintf(dst, "$%04X | 0x%02X%02X -> I += V%1X\n", at, code[0], code[1], LOW_NIBBLE(code[0])); break; }
                case 0x29: { sprintf(dst, "$%04X | 0x%02X%02X -> I = sprite_addr[V%1X]\n", at, code[0], code[1], LOW_NIBBLE(code[0])); break; }
                case 0x33: { sprintf(dst, "$%04X | 0x%02X%02X -> set_BCD(V%1X)\n", at, code[0], code[1], LOW_NIBBLE(code[0])); break; }
                case 0x55: { sprintf(dst, "$%04X | 0x%02X%02X -> reg_dump(V%1X, &I)\n", at, code[0], code[1], LOW_NIBBLE(code[0])); break; }
                case 0x65: { sprintf(dst, "$%04X | 0x%02X%02X -> reg_load(V%1X, &I)\n", at, code[0], code[1], LOW_NIBBLE(code[0])); break; }
            }
            break; 
        }
    }
}


static void unimplemented_instruction(uint8_t* code)
{
    printf("Unimplemented instruction -> %02X%02X\n", code[0], code[1]);
    exit(-1);
}


static void __chip8_draw(Chip8* chip8, uint8_t rx, uint8_t ry, uint8_t n)
{
    /*
    Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels 
    and a height of N pixels. Each row of 8 pixels is read as bit-coded 
    starting from memory location I; I value does not change after the 
    execution of this instruction. As described above, VF is set to 1 if 
    any screen pixels are flipped from set to unset when the sprite is 
    drawn, and to 0 if that does not happen
    */
   chip8->v[0xF] = 0x0;
   uint8_t x, y;
   for (uint32_t row = 0; row < n; ++row)
   {
       x = chip8->v[rx];
       y = chip8->v[ry] + row;
       if (y >= CHIP8_DISPLAY_HEIGHT) continue;
       uint8_t pixel_data = chip8->memory[chip8->I + row];
       
       for (int p_id = 0; p_id < 8; ++p_id)
       {
           if (x >= CHIP8_DISPLAY_WIDTH) continue;
           uint8_t pixel = ( pixel_data >> (7-p_id) ) & 0x1;
           if (chip8->VRAM[VRAM_AT(x, y)] && pixel)
           {
               chip8->v[0xF] = 0x1;
           }
           
           chip8->VRAM[VRAM_AT(x, y)] = chip8->VRAM[VRAM_AT(x, y)] ^ pixel;
           x++;
       }
   }
}


void chip8_execute(Chip8* chip8)
{
    uint8_t* code = &chip8->memory[chip8->pc];

    switch((code[0] & 0xF0) >> 4)
    {
        case 0x0: {
            uint16_t opcode = (code[0] << 8) | code[1];
            if (opcode == 0x00E0) // Display clear
            {
                memset(chip8->VRAM, 0x0, CHIP8_VRAM_SIZE);
                chip8->pc+=2;
            }
            else if (opcode == 0x00EE) // return
            {
                // Recover subroutine call address from the stack and set program counter to it
                // Decrease stack pointer and increase program counter
                chip8->sp--;
                chip8->pc = chip8->stack[chip8->sp];
                chip8->pc += 2;
            }
            else 
            {
                uint16_t nnn = opcode & 0x0FFF;
                unimplemented_instruction(code);
                chip8->pc += 2;
            }
            break;
        }
        case 0x1: {
            uint16_t goto_address = ((code[0] & 0x0F) << 8) | code[1];
            chip8->pc = goto_address;
            break;
        }
        case 0x2: { 
            uint16_t nnn = ((code[0] & 0x0F) << 8) | code[1];
            // Store sburoutine call address at current stack position
            // Increase stack pointer and set program counter to new subroutine address
            chip8->stack[chip8->sp] = chip8->pc;
            chip8->sp++;
            chip8->pc = nnn;
            break; 
        }
        case 0x3: { 
            uint8_t x = code[0] & 0x0F;
            uint8_t nn = code[1];
            if (chip8->v[x] == nn)
            {
                chip8->pc += 2;
            }
            chip8->pc += 2;
            break; 
        }
        case 0x4: { 
            uint8_t x = code[0] & 0x0F;
            uint8_t nn = code[1];
            if (chip8->v[x] != nn)
            {
                chip8->pc += 2;
            }
            chip8->pc += 2;
            break; 
        }
        case 0x5: {
            if ((code[1] & 0x0F) == 0)
            {
                uint8_t x = code[0] & 0x0F;
                uint8_t y = (code[1] & 0xF0) >> 4;
                if (chip8->v[x] == chip8->v[y])
                {
                    chip8->pc += 2;
                }
                chip8->pc += 2;
            }
            break; 
        }
        case 0x6: { 
            uint8_t x = code[0] & 0x0F;
            uint8_t nn = code[1];
            chip8->v[x] = nn;
            chip8->pc += 2;
            break; 
        }
        case 0x7: {
            uint8_t x = code[0] & 0x0F;
            uint8_t nn = code[1];
            chip8->v[x] += nn;
            chip8->pc += 2;
            break;
        }
        case 0x8: {
            uint8_t x = code[0] & 0x0F;
            uint8_t y = (code[1] & 0xF0) >> 4;
            switch(code[1] & 0x0F)
            {
                case 0x0: {
                    chip8->v[x] = chip8->v[y];
                    chip8->pc += 2;
                    break;
                }
                case 0x1: {
                    chip8->v[x] |= chip8->v[y];
                    chip8->pc += 2;
                    break;
                }
                case 0x2: {
                    chip8->v[x] &= chip8->v[y];
                    chip8->pc += 2;
                    break;
                }
                case 0x3: {
                    chip8->v[x] ^= chip8->v[y];
                    chip8->pc += 2;
                    break;
                }
                case 0x4: {
                    uint16_t result = chip8->v[x] + chip8->v[y];
                    chip8->v[x] = result & 0xFF;
                    chip8->v[0xF] = (result > 0xFF) ? 0x1 : 0x0;
                    chip8->pc += 2;
                    break;
                }
                case 0x5: {
                    uint16_t result = chip8->v[x] - chip8->v[y];
                    chip8->v[0xF] = (chip8->v[x] < chip8->v[y]) ? 0x0 : 0x1;
                    chip8->v[x] = result & 0xFF;
                    chip8->pc += 2;
                    break;
                }
                case 0x6: {
                    chip8->v[0xF] = chip8->v[x] & 0x01;
                    chip8->v[x] = chip8->v[x] >> 1;
                    chip8->pc += 2;
                    break;
                }
                case 0x7: {
                    uint16_t result = chip8->v[y] - chip8->v[x];
                    chip8->v[0xF] = (chip8->v[x] > chip8->v[y]) ? 0x0 : 0x1;
                    chip8->v[x] = result & 0xFF;
                    chip8->pc += 2;
                    break;
                }
                case 0xE: {
                    chip8->v[0xF] = (chip8->v[x] >> 7) & 0x1;
                    chip8->v[x] = chip8->v[x] << 1;
                    chip8->pc += 2;
                    break;
                }
            }
            break; 
        }
        case 0x9: { 
            if ((code[1] & 0x0F) == 0)
            {
                uint8_t x = code[0] & 0x0F;
                uint8_t y = (code[1] & 0xF0) >> 4;
                if (chip8->v[x] != chip8->v[y])
                {
                    chip8->pc += 2;
                }
                chip8->pc += 2;
            }
            break; 
        }
        case 0xA: { 
            uint16_t nnn = ((code[0] & 0x0F) << 8) | code[1];
            chip8->I = nnn;
            chip8->pc += 2;
            break; 
        }
        case 0xB: { 
            uint16_t nnn = ((code[0] & 0x0F) << 8) | code[1];
            chip8->pc = nnn + chip8->v[0];
            break; 
        }
        case 0xC: { 
            uint8_t x = code[0] & 0x0F;
            uint8_t nn = code[1];
            chip8->v[x] = rand() % (nn + 1);
            chip8->pc += 2;
            break; 
        }
        case 0xD: { 
            uint8_t x = code[0] & 0x0F;
            uint8_t y = (code[1] & 0xF0) >> 4;
            uint8_t n = code[1] & 0x0F;
            __chip8_draw(chip8, x, y, n);
            chip8->pc += 2;
            break; 
        }
        case 0xE: { 
            uint8_t x = code[0] & 0x0F;
            if (code[1] == 0x9E)
            {
                if (chip8->keyboard[chip8->v[x]] == 1)
                {
                    chip8->pc += 2;
                }
                chip8->pc += 2;
            }
            else if (code[1] == 0xA1)
            {
                if (chip8->keyboard[chip8->v[x]] == 0)
                {
                    chip8->pc += 2;
                }
                chip8->pc += 2; 
            }
            break; 
        }
        case 0xF: { 
            uint8_t x = code[0] & 0x0F;
            switch(code[1])
            {
                case 0x07: {
                    chip8->v[x] = chip8->delay_timer;
                    chip8->pc += 2;
                    break;
                }
                case 0x0A: {
                    static int awaiting_key = 0;
                    static int saved_keyboard[CHIP8_KEYBOARD_SIZE] = {0};

                    if (!awaiting_key)
                    {
                        awaiting_key = 1;
                        memcpy(&saved_keyboard, &chip8->keyboard, CHIP8_KEYBOARD_SIZE);
                    }
                    else
                    {
                        // Compare key by key, and check if one has changed from 0 to 1
                        int key_changed = -1;
                        for (int key = 0; key < CHIP8_KEYBOARD_SIZE; ++key)
                        {
                            if(!saved_keyboard[key] && chip8->keyboard[key])
                            {
                                key_changed = key;
                                break;
                            }
                        }
                        if (key_changed > -1)
                        {
                            awaiting_key = 0;
                            chip8->v[x] = key_changed & 0xFF;
                            chip8->pc += 2;
                        }
                    }
                    
                    break;
                }
                case 0x15: {
                    chip8->delay_timer = chip8->v[x];
                    chip8->pc += 2;
                    break;
                }
                case 0x18: {
                    chip8->sound_timer = chip8->v[x];
                    chip8->pc += 2;
                    break;
                }
                case 0x1E: {
                    chip8->I += chip8->v[x];
                    chip8->pc += 2;
                    break;
                }
                case 0x29: {
                    // NOTE[JCH]: Font data stored at 0x0 in memory, each sprite 5 bytes
                    chip8->I = 5*chip8->v[x];
                    chip8->pc += 2;
                    break;
                }
                case 0x33: {
                    /*
                    Stores the binary-coded decimal representation of VX, with 
                    the most significant of three digits at the address in I,
                     the middle digit at I plus 1, and the least significant digit 
                     at I plus 2. (In other words, take the decimal representation 
                     of VX, place the hundreds digit in memory at location in I, 
                     the tens digit at location I+1, and the ones digit at location I+2.); 
                    */
                   
                    uint8_t temp = chip8->v[x];
                    uint8_t hundreds = temp / 100;
                    temp -= hundreds*100;
                    uint8_t tens = temp / 10;
                    temp -= tens*10;

                    chip8->memory[chip8->I] = hundreds;
                    chip8->memory[chip8->I + 1] = tens;
                    chip8->memory[chip8->I + 2] = temp;
                    chip8->pc += 2;
                    break;
                }
                case 0x55: { // Dump
                    memcpy(&chip8->memory[chip8->I], &chip8->v, x+1);
                    chip8->pc += 2;
                    break;
                }
                case 0x65: { // Load
                    memcpy(&chip8->v, &chip8->memory[chip8->I], x+1);
                    chip8->pc += 2;
                    break;
                }
            }
            break; 
        }
        default: {
            unimplemented_instruction(code);
        }
    }
}