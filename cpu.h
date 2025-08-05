#ifndef CPU_H 
#define CPU_H

#include "global.h" 
#include <stdbool.h>

typedef uint8_t byte;
typedef uint16_t word;

// NOTE: v[0xF] is used as a flag by some instructions.
// NOTE: i is a memory address, so only the lowest 12 bits are typically used
// NOTE: the stack is an array of 16 16-bit values used to store the address 
// that the interpreter should return to when finished with a subroutine
typedef struct {
	// 8-bit registers
	byte v[0x10];           // 16 general purpose "V" registers
	byte delay_timer;       // delay timer register
	byte sound_timer;       // sound timer register
	byte stack_pointer;     // stack pointer register
	// 16-bit registers
	word index;             // index register
	word program_counter;   // program counter register
	// other data
	word stack[STACK_SIZE]; // for entering / exiting subroutines
	bool keys[NUM_KEYS];    // keyboard!
} Chip8Cpu;

// word opcode; // FIXME: Is this necessary?
byte memory[RAM_SIZE];

// functions
void c8_init(Chip8Cpu *const cpu);
void c8_reset(Chip8Cpu *const cpu);
void c8_push_stack(Chip8Cpu *const cpu, const word value);
void c8_pop_stack(Chip8Cpu *const cpu);

#endif // CPU_H
