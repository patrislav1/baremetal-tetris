#pragma once

#include <stdint.h>

#define SCREEN_SIZE_X 18
#define SCREEN_SIZE_Y 30
#define MAP_SIZE_X (SCREEN_SIZE_X - 2)
#define MAP_SIZE_Y (SCREEN_SIZE_Y - 2)

typedef int16_t coord_t;

typedef struct pos {
    coord_t x, y;
} pos_t;

typedef struct game_map {
    uint8_t block[SCREEN_SIZE_Y][SCREEN_SIZE_X];
} game_map_t;

void output_init(void);
void output_render(const game_map_t* map);
