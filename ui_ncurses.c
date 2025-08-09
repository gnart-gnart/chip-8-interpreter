#include <ncurses.h>
#include "ui.h"
#include "global.h"
#include "string.h"

bool prev_screen[SCREEN_WIDTH][SCREEN_HEIGHT];
WINDOW *screen_win;

// implementations
void ui_init(const byte width, const byte height) {
    initscr();
    raw();
    noecho();
    nodelay(stdscr, 1);
    curs_set(0);
    screen_win = newwin(height + 2, width + 2, 0, 0);
    box(screen_win, 0, 0);
    wrefresh(screen_win);
    memset(prev_screen, 0, sizeof prev_screen);
}

void ui_draw_screen(const bool screen[SCREEN_WIDTH][SCREEN_HEIGHT]) {
    for (byte y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            // update pixel only if it changed
            if (prev_screen[x][y] != screen[x][y]) {
                prev_screen[x][y] = screen[x][y];
                if (screen[x][y]) mvwaddch(screen_win, y + 1, x + 1, '#');
                else mvwaddch(screen_win, y + 1, x + 1, ' ');
            }
        }
    }
    box(screen_win, 0, 0);
    wrefresh(screen_win);
}

void ui_clear_screen(void) {
    erase();
    werase(screen_win);
    box(screen_win, 0, 0);
    wrefresh(screen_win);
}

void ui_deinit(void) {
    delwin(screen_win);
    endwin();
}

void ui_play_sound(void) {
    printf("\a");
}

