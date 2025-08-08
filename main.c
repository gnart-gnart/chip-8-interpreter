#include <unistd.h>
#include "global.h"
#include "chip8.h"
#include "ui.h"

int main(int argc, char *argv[]) {

    // initialize a chip8
    Chip8 my_chip8;
    c8_init(my_chip8);
    c8_load_rom(my_chip8);

    while (1) {
        c8_tick(my_chip8);
        ui_draw_screen(my_chip8.screen);
        ui_delay();
        usleep(1667);
    }

    return 0;
}
