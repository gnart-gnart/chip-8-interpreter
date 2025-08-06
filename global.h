#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdint.h>

#define SCREEN_WIDTH    64
#define SCREEN_HEIGHT   32
#define RAM_SIZE        4096
#define START_ADDR      0x200 // 512 bytes
#define STACK_SIZE      16
#define NUM_KEYS        16
#define NUM_V_REGS      16

typedef uint8_t byte;
typedef uint16_t word;

#endif // GLOBAL_H
