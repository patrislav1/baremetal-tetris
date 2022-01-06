#pragma once

#include <stdbool.h>

#include "output.h"
#include "util/minicurses.h"

typedef struct piece {
    mc_color_t color;
    struct rel_pos {
        signed dx : 4;
        signed dy : 4;
    } blocks[4];
} piece_t;

piece_t piece_get_random(void);
void piece_draw(const piece_t* piece, const pos_t pos, game_map_t* map);
bool piece_collision(const piece_t* piece, const pos_t pos, game_map_t* map);
coord_t piece_get_max_dy(const piece_t* piece);
void piece_rotate(piece_t* piece);
