# Journey
[toc]

## Introduction

The whole point of this file is to properly document the steps I have done in order to finish a CHIP-8 emulator. I have always wanted to write an emulator for the GameBoy console, but after doing some research on the internet I found it was easier as an initial emulator project to begin with something simpler, like the CHIP-8.

I have already done some previous work on this, so I do not start from scratch, but I think trying one last time to complete this emulator will provide me with a lot of knowledge and experience.


## The Journey


### First steps

I started by reading a bit the webpage of [Emulator 101](http://www.emulator101.com/welcome.html). It focuses first on the 8080 processor, in order to emulate the Space Invaders game, but that is perfect, because we will have instructions on what we need to do, but not exactly how to do it, which is the whole point of this exercise.


### Manual disassembly

After reaching the "Diving In" section, I selected a ROM I had some prior experience with, the `Maze [David Winter, 199x].ch8`. CHIP-8 ROMs can easily be found, for example [here](https://github.com/kripod/chip8-roms).

Let's see the contents of the ROM:

```
hexdump -v "Maze [David Winter, 199x].ch8" 

# NOTE: Little endian machine, so actual OpCodes are the other way around!

0000000 1ea2 01c2 0132 1aa2 14d0 0470 4030 0012
0000010 0060 0471 2031 0012 1812 4080 1020 4020
0000020 1080                                   
0000022

# NOTE: This way, you can see the individual bytes in the correct order

hexdump -v -C "Maze [David Winter, 199x].ch8" 
00000000  a2 1e c2 01 32 01 a2 1a  d0 14 70 04 30 40 12 00  |....2.....p.0@..|
00000010  60 00 71 04 31 20 12 00  12 18 80 40 20 10 20 40  |`.q.1 .....@ . @|
00000020  80 10                                             |..|
00000022
```

So, in the same way as in the webpage, having an OpCodes table with us (either [Wikipedia](https://en.wikipedia.org/wiki/CHIP-8) or the [Instruction Set](https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Instruction-Set) are OK), we will try to manually see what the program is trying to do. I think this is a good exercise, because not only can you get familiarized with the OpCode table (which, in this case, is just 35 instructions), you will see what the program actually does in pseudo-code.

As a reminder, because most programs start with program counter at `0x200`, that is the starting region for our program.


| Address | Instruction | Description |
| ------- | ----------- | ----------- |
| $200 | a21e | I = 0x21E |
| $202 | c201 | V2 = rand() % 0x01 |
| $204 | 3201 | Skip next instruction if(V2 == 0x01) |
| $206 | a21a | I = 0x21A |
| $208 | d014 | Draw(V0, V1, 4) |
| $20A | 7004 | V0 += 0x04 |
| $20C | 3040 | Skip next instruction if(V0 == 0x40) |
| $20E | 1200 | goto $200 |
| $210 | 6000 | V0 = 0x00 |
| $212 | 7104 | V1 += 0x04 |
| $214 | 3120 | Skip next instruction if(V1 == 0x20) |
| $216 | 1200 | goto $200 |
| $218 | 1218 | goto $218 |
| $21A | 8040 | V0 = V4 |
| $21C | 2010 | *($010)() |
| $21E | 2040 | *($040)() |
| $220 | 8010 | V0 = V1 |

If you have read what the OpCodes actually do and have generated the table from above on your own, you will see that everything after instruction at `$218` is data, not actual instructions for the processor.

So, if we were to write that code in some other programming language, it would look something like:

```c
uint16_t I; // Memory register (12 bits only)
uint8_t v[16]; // Registers

memset(v, 0x0, 16);
do {
    do {
        I = 0x21E;
        v[2] = rand() % 0x01;
        if(v[2] != 0x01)
        {
            I = 0x21A;
        }
        draw(v[0], v[1], 4);
        v[0] += 4;

    } while(v[0] != 0x40);

    v[0] = 0x00;
    v[1] += 0x04;

} while(v[1] != 0x20);

while(1);
```

So yeah, it is a double loop with a draw function, so you might have guessed (correctly) that we are drawing pixels (or groups of pixels) to the screen. We can even go a step further and say that the inner loop iterates over the columns of the screen, because it goes up until `v[0] != 0x40`, and `0x40` is `0d64`, which is the width of the screen, and the outer loop iterates over the rows of the screen. But let us focus on more emulator-related stuff.


### ROM loading

Before we even begin writing code for the disassembler, we need to first read a ROM from disk and load it into memory, starting at address `0x200`.

We first need to create the memory buffer for the program. Programs have a maximum size of `0x1000` bytes, so we will go with that:

```c
#define CHIP8_MEMORY_SIZE 0x1000

static uint8_t memory[CHIP8_MEMORY_SIZE];
```

Then, after initializing the memory buffer to zeroes (although it is not necessary, as `memory` is a static variable here), we will try to read the ROM file and load its contents onto the memory buffer:

```c
const char* rom_path = "data/chip8-roms/demos/Maze [David Winter, 199x].ch8";

memset(&memory, 0x0, sizeof(memory)/sizeof(memory[0]));

FILE* rom = fopen(rom_path, "rb");
if (!rom)
{
    printf("Rom file could not be loaded.\n");
    return -1;
}

fseek(rom, 0, SEEK_END);
size_t rom_size = ftell(rom);
fseek(rom, 0, SEEK_SET);

for (size_t i = 0; i < rom_size; ++i)
{
    fread(&memory[0x0200 + i], sizeof(uint8_t), 1, rom);
}

fclose(rom);
```

And that is about it, nothing fancy here yet. The whole program is just 38 lines of code:

```c
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#define CHIP8_MEMORY_SIZE 0x1000


static uint8_t memory[CHIP8_MEMORY_SIZE];


int main(int argc, char** argv)
{
    printf("CHIP-8 Emulator\n");

    memset(&memory, 0x0, sizeof(memory)/sizeof(memory[0]));

    const char* rom_path = "data/chip8-roms/demos/Maze [David Winter, 199x].ch8";

    FILE* rom = fopen(rom_path, "rb");
    if (!rom)
    {
        printf("Rom file could not be loaded.\n");
        return -1;
    }

    fseek(rom, 0, SEEK_END);
    size_t rom_size = ftell(rom);
    fseek(rom, 0, SEEK_SET);

    for (size_t i = 0; i < rom_size; ++i)
    {
        fread(&memory[0x0200 + i], sizeof(uint8_t), 1, rom);
    }

    fclose(rom);

    return 0;
}
```

Now that we have the ROM contents in memory, let us try to generate programatically the table from before.


### CHIP-8 Disassembler

The disassembler reads machine bytecode (what we obtained using `hexdump`) and outputs a table similar to the one we created. This is useful to have an understading of what the code does in pseudo-code, and also as a helping tool while debugging a CHIP-8 ROM.

Following our reference webpage, we will create a `disassemble` function.

I have first created a `Chip8` structure to hold everything and added a new definition:

```c
#define CHIP8_PROGRAM_START_LOCATION 0x0200

typedef struct {
    uint16_t pc;
    uint8_t memory[CHIP8_MEMORY_SIZE];
} Chip8;

static Chip8 chip8;
```

and right after closing the ROM file:

```c
while (chip8.pc < CHIP8_PROGRAM_START_LOCATION + rom_size)
{
    chip8.pc += disassemble_chip8(chip8.pc);
}
```

which will disassemble only the instructions loaded in memory, not the whole contents of the memory buffer.

An initial definition of the `disassemble` function could be something like this:

```c
static uint16_t disassemble_chip8(uint16_t at)
{
    uint8_t* code = &chip8.memory[at];
    printf("$%04X -> 0x%02X%02X\n", at, code[0], code[1]);
    return 2; // number of OPCODES read
}
```

One difference with respect to the reference webpage is that the indices for `code` are swapped. This is quite probably because in the webpage the code used to load the whole ROM is a one liner

```c
fread(buffer, fsize, 1, f); 
```

while I have opted for reading each individual byte:

```c
for (size_t i = 0; i < rom_size; ++i)
{
    fread(&memory[0x0200 + i], sizeof(uint8_t), 1, rom);
}
```

I don't know what is "better", but I suppose my approach makes the program completely independent of the endianness of the machine.

If we run the program, we can see that it properly outputs the first two columns of the table we created before.

```
CHIP-8 Emulator
$0200 -> 0xA21E
$0202 -> 0xC201
$0204 -> 0x3201
$0206 -> 0xA21A
$0208 -> 0xD014
$020A -> 0x7004
$020C -> 0x3040
$020E -> 0x1200
$0210 -> 0x6000
$0212 -> 0x7104
$0214 -> 0x3120
$0216 -> 0x1200
$0218 -> 0x1218
$021A -> 0x8040
$021C -> 0x2010
$021E -> 0x2040
$0220 -> 0x8010
```

We just need to differentiate now each instruction and output an additional column corresponding to the description of the opcode. A couple of instructions can be differentiated by checking the first 4 bits of the opcode, while others depend on the last 4 bits as well. We will take all of this into account while developing the disassembler: we will first filter by the first 4 bits, then do whatever needs to be done for each case:

```c
switch((code[0] & 0xF0) >> 4)
{
    // ...
}
```

Let us build some of this instructions.

For the case of the `goto` instruction (`1NNN`), we can write something like this:

```c
case 0x1: {
    uint16_t goto_address = ((code[0] & 0x0F) << 8) | code[1];
    printf("goto $%03X", goto_address);
    break;
}
```

I have added the `goto_address` intermediate variable for clarity. Because what the function does is just print the value, we could simply put the expression in the printf, but I think this helps while developing. If you want to remove these at a later stage, feel free to do so.

Now, the program should output something like

```
CHIP-8 Emulator
$0200 | 0xA21E -> 
$0202 | 0xC201 -> 
$0204 | 0x3201 -> 
$0206 | 0xA21A -> 
$0208 | 0xD014 -> 
$020A | 0x7004 -> 
$020C | 0x3040 -> 
$020E | 0x1200 -> goto $200
$0210 | 0x6000 -> 
$0212 | 0x7104 -> 
$0214 | 0x3120 -> 
$0216 | 0x1200 -> goto $200
$0218 | 0x1218 -> goto $218
$021A | 0x8040 -> 
$021C | 0x2010 -> 
$021E | 0x2040 -> 
$0220 | 0x8010 -> 
```

Our next instruction will be `7XNN`:

```c
case 0x7: {
    uint8_t x = code[0] & 0x0F;
    uint8_t nn = code[1];
    printf("V%1X += 0x%02X", x, nn);
    break;
}
```
```
CHIP-8 Emulator
$0200 | 0xA21E -> 
$0202 | 0xC201 -> 
$0204 | 0x3201 -> 
$0206 | 0xA21A -> 
$0208 | 0xD014 -> 
$020A | 0x7004 -> V0 += 0x04
$020C | 0x3040 -> 
$020E | 0x1200 -> goto $200
$0210 | 0x6000 -> 
$0212 | 0x7104 -> V1 += 0x04
$0214 | 0x3120 -> 
$0216 | 0x1200 -> goto $200
$0218 | 0x1218 -> goto $218
$021A | 0x8040 -> 
$021C | 0x2010 -> 
$021E | 0x2040 -> 
$0220 | 0x8010 ->
```

We can see how the table starts to get filled little by little. Let's jump right to the (my) solution for the `disassemble` function:

```c
static uint16_t disassemble_chip8(uint16_t at)
{
    uint8_t* code = &chip8.memory[at];
    printf("$%04X | 0x%02X%02X -> ", at, code[0], code[1]);

    switch((code[0] & 0xF0) >> 4)
    {
        case 0x0: {
            uint16_t opcode = (code[0] << 8) | code[1];
            if (opcode == 0x00E0) 
            {
                printf("disp_clear()");
            }
            else if ( opcode == 0x00EE)
            {
                printf("return");
            }
            else 
            {
                uint16_t nnn = opcode & 0x0FFF;
                printf("call subroutine at 0x%03X", nnn);
            }
            break;
        }
        case 0x1: {
            uint16_t goto_address = ((code[0] & 0x0F) << 8) | code[1];
            printf("goto $%03X", goto_address);
            break;
        }
        case 0x2: { 
            uint16_t nnn = ((code[0] & 0x0F) << 8) | code[1];
            printf("*(0x%03X)()", nnn);
            break; 
        }
        case 0x3: { 
            uint8_t x = code[0] & 0x0F;
            uint8_t nn = code[1];
            printf("skip if (V%1X == 0x%02X)", x, nn);
            break; 
        }
        case 0x4: { 
            uint8_t x = code[0] & 0x0F;
            uint8_t nn = code[1];
            printf("skip if (V%1X != 0x%02X)", x, nn);
            break; 
        }
        case 0x5: {
            if ((code[1] & 0x0F) == 0)
            {
                uint8_t x = code[0] & 0x0F;
                uint8_t y = (code[1] & 0xF0) >> 4;
                printf("skip if (V%1X == V%1X)", x, y);
            }
            break; 
        }
        case 0x6: { 
            uint8_t x = code[0] & 0x0F;
            uint8_t nn = code[1];
            printf("V%1X = 0x%02X", x, nn);
            break; 
        }
        case 0x7: {
            uint8_t x = code[0] & 0x0F;
            uint8_t nn = code[1];
            printf("V%1X += 0x%02X", x, nn);
            break;
        }
        case 0x8: {
            uint8_t x = code[0] & 0x0F;
            uint8_t y = (code[1] & 0xF0) >> 4;
            switch(code[1] & 0x0F)
            {
                case 0x0: {
                    printf("V%1X = V%1X", x, y);
                    break;
                }
                case 0x1: {
                    printf("V%1X |= V%1X", x, y);
                    break;
                }
                case 0x2: {
                    printf("V%1X &= V%1X", x, y);
                    break;
                }
                case 0x3: {
                    printf("V%1X ^= V%1X", x, y);
                    break;
                }
                case 0x4: {
                    printf("V%1X += V%1X", x, y);
                    break;
                }
                case 0x5: {
                    printf("V%1X -= V%1X", x, y);
                    break;
                }
                case 0x6: {
                    printf("V%1X >>= 1", x);
                    break;
                }
                case 0x7: {
                    printf("V%1X = V%1X - V%1X", x, y, x);
                    break;
                }
                case 0xE: {
                    printf("V%1X <<= 1", x);
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
                printf("skip if (V%1X != V%1X)", x, y);
            }
            break; 
        }
        case 0xA: { 
            uint16_t nnn = ((code[0] & 0x0F) << 8) | code[1];
            printf("I = 0x%03X", nnn);
            break; 
        }
        case 0xB: { 
            uint16_t nnn = ((code[0] & 0x0F) << 8) | code[1];
            printf("PC = V0 + 0x%03X", nnn);
            break; 
        }
        case 0xC: { 
            uint8_t x = code[0] & 0x0F;
            uint8_t nn = code[1];
            printf("V%1X = rand() & 0x%02X", x, nn);
            break; 
        }
        case 0xD: { 
            uint8_t x = code[0] & 0x0F;
            uint8_t y = (code[1] & 0xF0) >> 4;
            uint8_t n = code[1] & 0x0F;
            printf("draw(V%1X, V%1X, 0x%1X)", x, y, n);
            break; 
        }
        case 0xE: { 
            uint8_t x = code[0] & 0x0F;
            if (code[1] == 0x9E)
            {
                printf("skip if (is_key_pressed(V%1X))", x);
            }
            else if (code[1] == 0xA1)
            {
                printf("skip if (!is_key_pressed(V%1X))", x);  
            }
            break; 
        }
        case 0xF: { 
            uint8_t x = code[0] & 0x0F;
            switch(code[1])
            {
                case 0x07: {
                    printf("V%1X = delay_timer", x);
                    break;
                }
                case 0x0A: {
                    printf("V%1X = key_pressed", x);
                    break;
                }
                case 0x15: {
                    printf("delay_timer = V%1X", x);
                    break;
                }
                case 0x18: {
                    printf("sound_timer = V%1X", x);
                    break;
                }
                case 0x1E: {
                    printf("I += V%1X", x);
                    break;
                }
                case 0x29: {
                    printf("I = sprite_addr[V%1X]", x);
                    break;
                }
                case 0x33: {
                    printf("set_BCD(V%1X)", x);
                    break;
                }
                case 0x55: {
                    printf("reg_dump(V%1X, &I)", x);
                    break;
                }
                case 0x65: {
                    printf("reg_load(V%1X, &I)", x);
                    break;
                }
            }
            break; 
        }
    }
    printf("\n");

    return 2; // number of OPCODES read
}
```

OK, that is done. Our next step is to put this to good use while writing the actual CHIP-8 emulator.


### CHIP-8 Emulator

First, move almost everything we have already coded to a header and a source file, `chip8.h` and `chip8.c`. Then we can expand our Chip8 structure with more content:

```c
#define CHIP8_NUM_REGISTERS 0x10
#define CHIP8_MEMORY_SIZE 0x1000
#define CHIP8_PROGRAM_START_LOCATION 0x0200
#define CHIP8_DISPLAY_WIDTH 64
#define CHIP8_DISPLAY_HEIGHT 32


typedef struct {
    uint8_t v[CHIP8_NUM_REGISTERS]; // V0-VF registers
    uint16_t I; // Address register
    uint16_t pc; // Program counter
    uint8_t memory[CHIP8_MEMORY_SIZE]; // Program memory
    uint8_t VRAM[CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT]; // Video buffer
    uint16_t rom_size;
} Chip8;
```

Now we have sixteen 8-bit registers, the address register `I` and a video buffer, which will be used later to render its contents to the screen.

We will also create an interface for the outer program to interact with the CHIP-8:

```c
Chip8 chip8_new();

void chip8_init(Chip8* chip8);

int chip8_load_rom(Chip8* chip8, const char* rom_path);

void chip8_disassemble_all(Chip8* chip8);
uint16_t chip8_disassemble_at(Chip8* chip8, uint16_t at);
```

Now we need to emulate the current instruction the program is pointing at. To do so, we will call a function that we will call `execute`, to which we will pass the `Chip8` data object. Execute select the function to call. For that, we will need to create a similar switch statement to the one from the `disassemble` function. The `execute` function will be called in an infinite loop, until we decide to quit the program.

To implement the `execute` function, it is best to start with the default behavior, which will trigger an early exit of the program and will notify us that the selected OpCode is currently not implemented.

```c
void chip8_execute(Chip8* chip8)
{
    uint8_t* code = &chip8->memory[chip8->pc];

    switch((code[0] & 0xF0) >> 4)
    {
        default: {
            printf("Unimplemented instruction -> %02X%02X\n", code[0], code[1]);
            exit(-1);
        }
    }
}
```

It is just then a matter of implementing the instructions one by one, making sure the program runs properly until the next unimplemented instruction is found.

Regarding the stack, it will be created as an array with fixed size, along with a stack pointer. In there, memory addresses of calls to `2NNN` will be stored, and then retrieved with `00EE`.

The keyboard inputs are set from outside the CHIP-8 emulator. Instructions for checking whether a key is pressed or not are easy to implement, but waiting for a key press will be left for a later point in the emulation (until a ROM requires it).

The BCD instruction could be coded with something like this:

```c
uint8_t temp = x;
uint8_t hundreds = x % 100;
x -= hundreds*100;
uint8_t tens = x % 10;
x -= tens*10;

chip8->memory[chip8->I] = hundreds;
chip8->memory[chip8->I + 1] = tens;
chip8->memory[chip8->I + 2] = x;
```

I think we are ready to start drawing to the screen.


### CHIP-8 Screen

We will be using SDL to render to the screen. The CHIP-8 has a screen of 64x32 pixels, so that will be our rendering target. Our actual screen will be a few times bigger, for example ten times, having a resolution of 640x320.

Rendering to the screen is quite easy. First, we need a screen and a renderer:

```c
SDL_Window* window = SDL_CreateWindow(
    "CHIP-8 Emulator", 
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
    WINDOW_WIDTH, WINDOW_HEIGHT, 
    0);

SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
```

That is our view to the application. Now, we need the CHIP-8 screen, which has smaller dimensions:

```c
SDL_Texture* screen_texture = SDL_CreateTexture(
        renderer, 
        SDL_PIXELFORMAT_RGBA32, 
        SDL_TEXTUREACCESS_TARGET, 
        CHIP8_DISPLAY_WIDTH, CHIP8_DISPLAY_HEIGHT);
```

To render to the target texture, we will set the texture as the rendering target. Firstly, we clear the texture. Then, we just need to go through the CHIP-8's VRAM, pixel by pixel, and check whether it is active or not. If it is, then we just render it.


```c
SDL_SetRenderTarget(renderer, screen_texture);
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    for (int i = 0; i < CHIP8_VRAM_SIZE; ++i)
    {
        if (chip8.VRAM[i])
        {
            SDL_RenderDrawPoint(renderer, i % CHIP8_DISPLAY_WIDTH, i / CHIP8_DISPLAY_WIDTH);
        }
    }
}
```

After the rendering is done, we reset the rendering target to the actual screen and we copy the texture data to the screen.

```c
SDL_SetRenderTarget(renderer, NULL);
SDL_RenderCopy(renderer, screen_texture, NULL, NULL);
SDL_RenderPresent(renderer);
```

And that's it!


### CHIP-8 Input Support

This is also easy. Our CHIP-8 structure already contains the 16 keys in an array:

```c
typedef struct {
    // ...
    int keyboard[CHIP8_KEYBOARD_SIZE]; // Pressed state
} Chip8;
```

So we just need to update it with whatever inputs we want.  However, it is not as easy as getting whatever event SDL throws at us and updating the keyboard with it, because the CHIP-8 works on a `is_key_just_pressed` basis.

We need to create our own input module, which tracks which keys are being pressed, held and released for that giving frame. After doing so, it is really straightforward how to update the keyboard state:

```c
chip8.keyboard[0x0] = input_is_key_pressed(SDLK_0);
chip8.keyboard[0x1] = input_is_key_pressed(SDLK_1);
chip8.keyboard[0x2] = input_is_key_pressed(SDLK_2);
chip8.keyboard[0x3] = input_is_key_pressed(SDLK_3);
chip8.keyboard[0x4] = input_is_key_pressed(SDLK_4);
chip8.keyboard[0x5] = input_is_key_pressed(SDLK_5);
chip8.keyboard[0x6] = input_is_key_pressed(SDLK_6);
chip8.keyboard[0x7] = input_is_key_pressed(SDLK_7);
chip8.keyboard[0x8] = input_is_key_pressed(SDLK_8);
chip8.keyboard[0x9] = input_is_key_pressed(SDLK_9);
chip8.keyboard[0xA] = input_is_key_pressed(SDLK_a);
chip8.keyboard[0xB] = input_is_key_pressed(SDLK_b);
chip8.keyboard[0xC] = input_is_key_pressed(SDLK_c);
chip8.keyboard[0xD] = input_is_key_pressed(SDLK_d);
chip8.keyboard[0xE] = input_is_key_pressed(SDLK_e);
chip8.keyboard[0xF] = input_is_key_pressed(SDLK_f);
```