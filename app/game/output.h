#pragma once

#include <stdbool.h>
#include <stdint.h>

#define CHARS_PER_BLOCK 2
#define BACKGROUND_COLOR mc_color_default

#define MAP_SIZE_X 18
#define MAP_SIZE_Y 25

typedef int16_t coord_t;

typedef struct pos {
    coord_t x, y;
} pos_t;

typedef struct game_map {
    uint8_t block[MAP_SIZE_Y][MAP_SIZE_X];
} game_map_t;

void output_init(void);
void output_render(const game_map_t* map);
void output_text_box(const char** msgs);
void output_key_bindings(bool show);
void update_score_and_level(unsigned int score, unsigned int level);
