#pragma once

#include <stdint.h>

#define CHARS_PER_BLOCK 2
#define BACKGROUND_COLOR mc_color_default

#define MAP_SIZE_X 18
#define MAP_SIZE_Y 30

typedef int16_t coord_t;

typedef struct pos {
    coord_t x, y;
} pos_t;

typedef struct game_map {
    uint8_t block[MAP_SIZE_Y][MAP_SIZE_X];
} game_map_t;

void output_init(void);
void output_render(const game_map_t* map);
void output_game_msg(coord_t y, const char* str);
