#ifndef UI_H
#define UI_H

#include "global.h"

void ui_init(const byte width, const byte height);
void ui_draw_screen(const bool screen[SCREEN_WIDTH][SCREEN_HEIGHT]);
void ui_deinit(void);
void ui_clear_screen(void);
void ui_play_sound(void);

#endif // UI_H
