#include "output.h"

#include <string.h>

#include "util/minicurses.h"

static game_map_t current_screen;

#define CHARS_PER_BLOCK 2
#define BACKGROUND_COLOR mc_color_default

void output_init(void)
{
    mc_initscr();
    mc_setcursor(false);
    mc_set_bg(BACKGROUND_COLOR);

    memset(&current_screen, 0, sizeof(current_screen));

    coord_t y = 0;
    mc_move(0, y++);
    mc_putch(mc_sym_ulcorner);
    coord_t n = MAP_SIZE_X * CHARS_PER_BLOCK;
    while (n--) {
        mc_putch(mc_sym_hline);
    }
    mc_putch(mc_sym_urcorner);
    n = MAP_SIZE_Y;
    while (n--) {
        mc_move(0, y++);
        mc_putch(mc_sym_vline);
        coord_t m = MAP_SIZE_X * CHARS_PER_BLOCK;
        while (m--) {
            mc_putch(' ');
        }
        mc_putch(mc_sym_vline);
    }
    mc_move(0, y++);
    mc_putch(mc_sym_llcorner);
    n = MAP_SIZE_X * CHARS_PER_BLOCK;
    while (n--) {
        mc_putch(mc_sym_hline);
    }
    mc_putch(mc_sym_lrcorner);
}

void output_render(const game_map_t* map)
{
    for (coord_t y = 0; y < MAP_SIZE_Y; y++) {
        for (coord_t x = 0; x < MAP_SIZE_X; x++) {
            if (map->block[y][x] != current_screen.block[y][x]) {
                mc_move(x * CHARS_PER_BLOCK + 1, y + 1);
                mc_color_t c = map->block[y][x];
                mc_set_bg(c ? c : BACKGROUND_COLOR);
                for (int i = 0; i < CHARS_PER_BLOCK; i++) {
                    mc_putch(' ');
                }
                current_screen.block[y][x] = map->block[y][x];
            }
        }
    }
}
