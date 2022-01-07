#include "output.h"

#include <string.h>

#include "macros.h"
#include "util/minicurses.h"

#define SCREEN_SIZE_X (MAP_SIZE_X * CHARS_PER_BLOCK) + 2
#define SCREEN_SIZE_Y MAP_SIZE_Y + 2

static struct {
    uint8_t character[SCREEN_SIZE_Y][SCREEN_SIZE_X];
} screen_map;

void output_init(void)
{
    mc_initscr();
    mc_setcursor(false);
    mc_set_bg(BACKGROUND_COLOR);

    memset(&screen_map, 0, sizeof(screen_map));

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
            for (int i = 0; i < CHARS_PER_BLOCK; i++) {
                if (map->block[y][x] != screen_map.character[y][x * CHARS_PER_BLOCK + i]) {
                    mc_move(x * CHARS_PER_BLOCK + 1 + i, y + 1);
                    mc_color_t c = map->block[y][x];
                    mc_set_bg(c ? c : BACKGROUND_COLOR);
                    mc_putch(' ');
                    screen_map.character[y][x * CHARS_PER_BLOCK + i] = map->block[y][x];
                }
            }
        }
    }
}

void output_game_msg(coord_t y, const char* str)
{
    coord_t x = MAX(((int)SCREEN_SIZE_X - (int)strlen(str)) / 2, 0);
    mc_move(x, y);
    mc_putstr(str);
    // Invalidate area, so it gets refreshed in output_render()
    for (coord_t xc = x; xc <= x + strlen(str) && xc < MAP_SIZE_X; xc++) {
        // Magic value never used in the actual map
        screen_map.character[y][xc] = 0xff;
    }
}
