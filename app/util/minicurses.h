#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum mc_keycodes {
    key_backspace = '\b',
    key_escape = 0x1b,
    key_down = 0x80,
    key_up = 0x81,
    key_left = 0x82,
    key_right = 0x83,
    key_home = 0x84,
    key_delete = 0x85,
    key_insert = 0x86,
    key_pg_down = 0x87,
    key_pg_up = 0x88,
    key_end = 0x89,
    key_f1 = 0x8b,
    key_f2 = 0x8c,
    key_f3 = 0x8d,
    key_f4 = 0x8e,
    key_f5 = 0x8f,
    key_f6 = 0x90,
    key_f7 = 0x91,
    key_f8 = 0x92,
    key_f9 = 0x93,
    key_f10 = 0x94
} mc_keycodes_t;

typedef enum mc_symbols {
    mc_sym_diamond = 0x80,
    mc_sym_ckboard = 0x81,
    mc_sym_degree = 0x86,
    mc_sym_plminus = 0x87,
    mc_sym_lrcorner = 0x8a,
    mc_sym_urcorner = 0x8b,
    mc_sym_ulcorner = 0x8c,
    mc_sym_llcorner = 0x8d,
    mc_sym_plus = 0x8e,
    mc_sym_hline_1 = 0x8f,
    mc_sym_hline_3 = 0x90,
    mc_sym_hline_5 = 0x91,
    mc_sym_hline = mc_sym_hline_5,
    mc_sym_hline_7 = 0x92,
    mc_sym_hline_9 = 0x93,
    mc_sym_ltee = 0x94,
    mc_sym_rtee = 0x95,
    mc_sym_btee = 0x96,
    mc_sym_ttee = 0x97,
    mc_sym_vline = 0x98,
    mc_sym_lequal = 0x99,
    mc_sym_gequal = 0x9a,
    mc_sym_sterling = 0x9d,
    mc_sym_bullet = 0x9e,
} mc_symbols_t;

#define BIT(x) (1 << x)

typedef enum mc_attr {
    mc_attr_normal = 0,
    mc_attr_bold = BIT(1),
    mc_attr_dim = BIT(2),
    mc_attr_italic = BIT(3),
    mc_attr_underline = BIT(4),
    mc_attr_blink = BIT(5),
    mc_attr_reverse = BIT(7),
} mc_attr_t;

typedef enum mc_color {
    mc_color_black = 0,
    mc_color_red = 1,
    mc_color_green = 2,
    mc_color_brown = 3,
    mc_color_blue = 4,
    mc_color_magenta = 5,
    mc_color_cyan = 6,
    mc_color_white = 7,
    mc_color_default = 9,
} mc_color_t;

void mc_initscr(void);  // init screen
void mc_exitscr(void);  // exit screen

void mc_move(uint8_t x, uint8_t y);     // move cursor to position starting at (0, 0)
void mc_setattr(mc_attr_t attr);        // set attribute(s)
void mc_set_fg(mc_color_t foreground);  // set foreground
void mc_set_bg(mc_color_t background);  // set background

void mc_clear(void);              // clear screen
void mc_cleartoeol(void);         // clear from current column to end of row
void mc_deleterow(void);          // delete row at current cursor position
void mc_setcursor(bool visible);  // set cursor to: 0=invisible 1=normal 2=very visible
int mc_getch(void);
void mc_putch(unsigned char c);
void mc_putstr(const char* str);
