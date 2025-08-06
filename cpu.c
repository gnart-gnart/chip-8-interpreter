#include <string.h>
#include "cpu.h"
#include "font.h"

void c8_init(Chip8Cpu *const cpu) {
	c8_zero(cpu);
	cpu->program_counter = START_ADDR;
    memcpy(cpu->memory, fontset, FONTSET_SIZE); // write default font to memory
}

void c8_zero(Chip8Cpu *const cpu) {
	// reset V registers
	cpu->v[0x0] = 0;
	cpu->v[0x1] = 0;
	cpu->v[0x2] = 0;
	cpu->v[0x3] = 0;
	cpu->v[0x4] = 0;
	cpu->v[0x5] = 0;
	cpu->v[0x6] = 0;
	cpu->v[0x7] = 0;
	cpu->v[0x8] = 0;
	cpu->v[0x9] = 0;
	cpu->v[0xA] = 0;
	cpu->v[0xB] = 0;
	cpu->v[0xC] = 0;
	cpu->v[0xD] = 0;
	cpu->v[0xE] = 0;
	cpu->v[0xF] = 0;
	// reset other registers
	cpu->delay_timer = 0;
	cpu->sound_timer = 0;
	cpu->stack_pointer = 0;
	cpu->index = 0;
	// other data
	memset(cpu->stack, 0, sizeof(cpu->stack));
	memset(cpu->keys, false, sizeof(cpu->keys));
}

void c8_push_stack(Chip8Cpu *const cpu, const word value) {
	cpu->stack_pointer++;
	cpu->stack[cpu->stack_pointer] = value;
}

void c8_pop_stack(Chip8Cpu *const cpu) {
	cpu->stack[cpu->stack_pointer] = 0;
	cpu->stack_pointer--;
}

void c8_tick(Chip8Cpu *const cpu) {
    // fetch opcode (which is the current two bytes)
    word opcode = cpu->memory[cpu->program_counter] << 8 |
        cpu->memory[cpu->program_counter + 1];
    // increment program counter to next two bytes
    cpu->program_counter += 2;
    // execute instruction

    // decrement timers
    if (cpu->delay_timer > 0) cpu->delay_timer--;
    if (cpu->sound_timer > 0) {
        cpu->sound_timer--;
        // TODO: Play a sound!
    }
}
