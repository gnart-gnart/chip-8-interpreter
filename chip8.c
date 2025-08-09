#include <string.h>
#include <stdio.h>
#include <stdlib.h> // for rand
#include "chip8.h"
#include "font.h"
#include "ui.h"

// helper functions
static inline void c8_skip(Chip8 *const chip8) {
	chip8->program_counter += 2;
}

static inline void c8_wait(Chip8 *const chip8) {
	chip8->program_counter -= 2;
}

// return the k released this tick with the smallest index
// returns NUM_KEYS if no key has released this tick
static inline byte c8_key_released(Chip8 *const chip8) {
	for (byte k = 0; k < NUM_KEYS; k++) {
		if (keys_prev[k] && !keys[k]) return k;
	}
	return NUM_KEYS;
}

// returns true if there is a collision between a sprite pixel and a screen pixel that are both on
static inline bool draw_sprite(const byte height, const byte vx, const byte vy, const byte *const sprite_addr) {
	bool collision = false;
	// iterate through each row of the sprite (i.e., each byte)
	for (int row = 0; row < height; row++) {
		byte sprite_byte = memory[sprite_addr + row];
		// iterate through each bit of the sprite byte
		for (byte bit = 0; bit < 8; bit++) {
			byte screen_x = vx + bit;
			byte screen_y = vy + row;
			bool sprite_pixel = (sprite_byte >> (7 - bit)) & 1;
			if (sprite_pixel) {
				if (screen[x][y]) {
					collision = true;
				}
				screen[x][y] = !screen[x][y];
			}
		}
	}
	return collision;
}

// implementations
void c8_init(Chip8 *const chip8) {
	c8_zero(chip8);
	chip8->program_counter = START_ADDR;
    memcpy(chip8->memory, fontset, FONTSET_SIZE); // write default font to memory
	ui_init(SCREEN_WIDTH, SCREEN_HEIGHT);
}

void c8_zero(Chip8 *const chip8) {
	// reset V registers
	chip8->v[0x0] = 0;
	chip8->v[0x1] = 0;
	chip8->v[0x2] = 0;
	chip8->v[0x3] = 0;
	chip8->v[0x4] = 0;
	chip8->v[0x5] = 0;
	chip8->v[0x6] = 0;
	chip8->v[0x7] = 0;
	chip8->v[0x8] = 0;
	chip8->v[0x9] = 0;
	chip8->v[0xA] = 0;
	chip8->v[0xB] = 0;
	chip8->v[0xC] = 0;
	chip8->v[0xD] = 0;
	chip8->v[0xE] = 0;
	chip8->v[0xF] = 0;
	// reset other registers
	chip8->delay_timer = 0;
	chip8->sound_timer = 0;
	chip8->stack_pointer = 0;
	chip8->index = 0;
	// other data
	memset(chip8->stack, 0, sizeof(chip8->stack));
	memset(chip8->keys, false, sizeof(chip8->keys));
	memset(chip8->keys_prev, false, sizeof(chip8->keys_prev));
	memset(chip8->screen, false, sizeof(chip8->screen));
}

void c8_push_stack(Chip8 *const chip8, const word value) {
	chip8->stack_pointer++;
	chip8->stack[chip8->stack_pointer] = value;
}

void c8_pop_stack(Chip8 *const chip8) {
	chip8->stack[chip8->stack_pointer] = 0;
	chip8->stack_pointer--;
}

void c8_load_rom(Chip8 *const chip8, const char* fname) {
	FILE* rom_file = fopen(fname, "rb");
	if (rom_file == NULL) perror("could not open file: %s", fname);
	
	fseek(rom_file, 0, SEEK_END);
    long file_size = ftell(rom_file);
    fseek(rom_file, 0, SEEK_SET); // Reset file pointer to the beginning

	// Assuming 'chip8_memory' is your emulator's memory array (e.g., unsigned char memory[4096];)
    // And 'CHIP8_ROM_START_ADDRESS' is defined as 0x200
    size_t bytes_read = fread(&chip8->memory[START_ADDR], 1, file_size, rom_file);
    if (bytes_read != file_size) {
        // Handle error: not all bytes were read
        fprintf(stderr, "Error reading ROM file: Expected %ld bytes, read %zu bytes.\n", file_size, bytes_read);
    }

	fclose(rom_file);
}

void c8_tick(Chip8 *const chip8) {
    // fetch opcode (which is the current two bytes)
    word opcode = chip8->memory[chip8->program_counter] << 8 |
        chip8->memory[chip8->program_counter + 1];
    // increment program counter to next two bytes
	c8_skip(chip8);
	// execute instruction
    c8_execute(chip8, opcode);
    // decrement timers
    if (chip8->delay_timer > 0) chip8->delay_timer--;
    if (chip8->sound_timer > 0) {
        chip8->sound_timer--;
		ui_play_sound();
	}
	memcpy(chip8->keys_prev, chip8->keys, sizeof(chip8->keys));
}

// see https://chip8.gulrak.net/ for documentation
void c8_execute(Chip8 *const chip8, const word opcode) {
	// opcode can be split into various digit combos for later purposes	
	word nnn = (opcode & 0x0FFF); 		// last 3 digits of opcode
	byte nn = (byte) opcode;			// last 2 digits of opcode
	byte n = opcode & 0x000F;			// last digit of opcode
	byte x = (opcode & 0x0F00) >> 8; 	// 2nd digit of opcode
	byte y = (opcode & 0x00F0) >> 4;	// 3rd digit of opcode
	byte a = (opcode & 0xF000) >> 12;	// first digit
	byte v[NUM_V_REGS] = chip8->v;
	// FIXME: might need to be byte* v = chip8->v;
	// essentially trying to rename the array

	switch (a) {

		case 0x0:
			switch (nnn) {
				case 0x0E0:
					ui_clear_screen();
					break;
				case 0x0EE:
					chip8->pop_stack(chip8);
					break;
				default:
					goto unrecognized_instr;
					break;
			}
			break;

		case 0x1:
			chip8->program_counter = nnn;
			break;
		
		case 0x2:
			chip8->c8_push_stack(chip8, nnn);
			break;

		case 0x3:
			if (v[x] == nn) c8_skip(chip8);
			break;
		
		case 0x4:
			if (v[x] != nn) c8_skip(chip8);
			break;
		
		case 0x5:
			if (v[x] == v[y]) c8_skip(chip8);
			break;
		
		case 0x6:
			v[x] = nn;
			break;
		
		case 0x7:
			v[x] += nn;
			break;

		case 0x8:
			switch (n) {
				case 0x0:
					v[x] = v[y];
					break;
				case 0x1:
					v[x] |= v[y];
					break;
				case 0x2:
					v[x] &= v[y];
					break;
				case 0x3:
					v[x] ^= v[y];
					break;
				case 0x4:
					byte sum = v[x] + v[y];
					v[0xF] = (sum < v[x]); 		// overflow?
					v[x] = sum;
					break;
				case 0x5:
					v[0xF] = (v[x] >= v[y]); 	// didn't borrow?
					v[x] -= v[y];
					break;
				case 0x6:
					v[0xF] = (v[x] & 1); 		// least significant bit
					v[x] >>= v[y];
					break;
				case 0x7:
					v[0xF] = (v[x] <= v[y]); 	// didn't borrow?
					v[x] = v[y] - v[x];
					break;
				case 0xE:
					v[0xF] = (v[x] >> 7) & 1; 	// most significant bit
					v[x] <<= v[y];
					break;
				default:
					goto unrecognized_instr;
					break;
			}
			break;

		case 0x9:
			if (n == 0) {
				if (v[x] != v[y]) c8_skip(c8_skip);
			}
			else {
				goto unrecognized_instr;
			}
			break;
		
		case 0xA:
			chip8->index = nnn;
			break;

		case 0xB:
			chip8->program_counter = nnn + v[0];
			break;

		case 0xC:
			v[x] = (rand() % 256) & nn;
			break;

		case 0xD:
			if (draw_sprite(n, v[x], v[y], chip8->index)) v[0xF] = 1;
			break;

		case 0xE:
			switch (nn) {
				case 0x9E:
					if (chip8->keys[v[x]]) c8_skip(chip8);
					break;
				case 0xA1:
					if (!chip8->keys[v[x]]) c8_skip(chip8);
					break;
				default:
					goto unrecognized_instr;
					break;
			}
			break;
		
		case 0xF:
			switch (nn) {
				case 0x07:
					v[x] = chip8->delay_timer;
					break;
				case 0x0A:;
					byte key = c8_key_released(chip8);
					if (key != NUM_KEYS) {
						v[x] = key;
					}
					else {
						c8_wait(chip8);
					}
					break;
				case 0x15:
					chip8->delay_timer = v[x];
					break;
				case 0x18:
					chip8->sound_timer = v[x];
					break;
				case 0x1E:
					chip8->index += v[x];
					break;
				case 0x29:
					chip8->index = fontset[v[x]];
					break;
				case 0x33:; // convert v[x] to 3-digit decimal format
					byte x = v[x];
					chip8->memory[chip8->index] = (x - (x % 100)) / 100;
					x -= chip8->memory[chip8->index] * 100;
					chip8->memory[chip8->index + 1] = (x - (x % 10)) / 10;
					x -= chip8->memory[chip8->index + 1] * 10;
					chip8->memory[chip8->index + 2] = x; // FIXME: i have no idea if this is correct
					break;
				case 0x55:
					memcpy(chip8->v, chip8->memory[chip8->index], x + 1);
					chip8->index += x + 1;
					break;
				case 0x65:
					memcpy(chip8->memory[chip8->index], chip8->v, x + 1);
					chip8->index += x + 1;
					break;
				default:
					goto unrecognized_instr;
					break;
			}
		default:
			goto unrecognized_instr;
			break;
	}

	return;
	unrecognized_instr:
		printf("Chip8: Yo bruh i dont recognize this: %x wtf?\n", opcode);	
}

