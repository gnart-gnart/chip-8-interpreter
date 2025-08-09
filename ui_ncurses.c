#include <ncurses.h>
#include "ui.h"
#include "global.h"
// helper functions


// implementations
void ui_init(const byte width, const byte height) {
  initscr();
  raw();
  noecho();
  nodelay(stdscr, 1);
  curs_set(0);
}

void ui_draw_screen(const bool screen[SCREEN_WIDTH][SCREEN_HEIGHT]) {
    erase();
    for (byte y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            if (screen[x][y]) mvaddch(y, x, '#');
            else mvaddch(y, x, ' ');
        }
    }
    refresh();
}

void ui_clear_screen(void) {
    erase();
    refresh();
}

void ui_deinit(void) {
    endwin();
}

void ui_play_sound(void) {
    printf("\a");
}

