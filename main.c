#include <unistd.h>
#include <stdio.h>
#include "global.h"
#include "chip8.h"
#include "ui.h"

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Intended use: ./chip8 <path_to_rom>");
        return 1;
    }

    char* fname = argv[1];

    // initialize a chip8
    Chip8 my_chip8;
    c8_init(&my_chip8);
    c8_load_rom(&my_chip8, fname);

    while (1) {
        c8_tick(&my_chip8);
        ui_draw_screen(my_chip8.screen);
        ui_delay();
        usleep(1667);
    }

    return 0;
}
