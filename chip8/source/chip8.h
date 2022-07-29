#include <stdint.h>

#define CHIP8_NUM_REGISTERS 0x10
#define CHIP8_MEMORY_SIZE 0x1000
#define CHIP8_PROGRAM_START_LOCATION 0x0200
#define CHIP8_DISPLAY_WIDTH 64
#define CHIP8_DISPLAY_HEIGHT 32
#define CHIP8_VRAM_SIZE CHIP8_DISPLAY_WIDTH*CHIP8_DISPLAY_HEIGHT
#define CHIP8_STACK_SIZE 16
#define CHIP8_KEYBOARD_SIZE 16
#define CHIP8_DELAY_TIMER_FREQ 60


#define VRAM_AT(x, y) ((x) + (y) * CHIP8_DISPLAY_WIDTH)


typedef struct _Chip8 {
    uint8_t v[CHIP8_NUM_REGISTERS]; // V0-VF registers
    uint16_t I; // Address register
    uint16_t pc; // Program counter
    uint8_t memory[CHIP8_MEMORY_SIZE]; // Program memory
    uint8_t VRAM[CHIP8_VRAM_SIZE]; // Video buffer
    uint16_t rom_size;
    uint16_t sp; // Stack pointer
    uint16_t stack[CHIP8_STACK_SIZE];
    uint8_t delay_timer;
    uint8_t sound_timer;
    int keyboard[CHIP8_KEYBOARD_SIZE]; // Pressed state
} Chip8;


// Returns a new Chip8 object
Chip8* chip8_new();
// Deletes existing Chip8 object
void chip8_delete(Chip8* ch8);

// Initializes the whole Chip8 object
void chip8_init(Chip8* chip8);

// Load given ROM from given path into the Chip8 memory
// Returns 0 if OK, otherwise -1
int chip8_load_rom(Chip8* chip8, const char* rom_path);

#if 0
void chip8_disassemble_all(Chip8* chip8);
#endif

// Write disassembly of given instruction to pointer
void chip8_disassemble_at(Chip8* chip8, uint16_t at, char* dst);

// Performs an execution cycle of the Chip8
void chip8_execute(Chip8* chip8);