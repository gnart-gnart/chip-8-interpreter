#ifndef UI_H
#define UI_H

#include "global.h"

void ui_init(const byte width, const byte height);
void ui_toggle_pixel(const byte x, const byte y);
void ui_clear_screen(void);
void ui_play_sound(void);
void ui_deinit(void);

#endif // UI_H
