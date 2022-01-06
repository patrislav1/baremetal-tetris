#include "piece.h"

#include <stddef.h>

#include "macros.h"
#include "util/prng.h"

static const piece_t piece_templates[] = {
    // ##
    // ##
    {mc_color_brown, {{0, 0}, {1, 0}, {0, 1}, {1, 1}}},
    // ####
    {mc_color_cyan, {{-1, 0}, {0, 0}, {1, 0}, {2, 0}}},
    // ##
    //  ##
    {mc_color_red, {{-1, 0}, {0, 0}, {0, 1}, {1, 1}}},
    //  ##
    // ##
    {mc_color_green, {{-1, 1}, {0, 1}, {0, 0}, {1, 0}}},
    // ###
    //  #
    {mc_color_magenta, {{-1, 0}, {0, 0}, {1, 0}, {0, 1}}},
    // #
    // ###
    {mc_color_blue, {{-1, -1}, {-1, 0}, {0, 0}, {1, 0}}},
    //   #
    // ###
    {mc_color_blue, {{-1, 0}, {0, 0}, {1, 0}, {1, -1}}},
};

piece_t piece_get_random(void)
{
    piece_t p = piece_templates[prng_next() % NUM_ELEMS(piece_templates)];
    for (int i = 0; i < prng_next() % 4; i++) {
        piece_rotate(&p);
    }
    return p;
}

void piece_draw(const piece_t* piece, const pos_t pos, game_map_t* map)
{
    for (size_t i = 0; i < NUM_ELEMS(piece->blocks); i++) {
        const pos_t p = {
            pos.x + piece->blocks[i].dx,
            pos.y + piece->blocks[i].dy,
        };
        if (p.x >= 0 && p.x < MAP_SIZE_X && p.y >= 0 && p.y < MAP_SIZE_Y) {
            map->block[p.y][p.x] = piece->color;
        }
    }
}

bool piece_collision(const piece_t* piece, const pos_t pos, game_map_t* map)
{
    for (size_t i = 0; i < NUM_ELEMS(piece->blocks); i++) {
        const pos_t p = {
            pos.x + piece->blocks[i].dx,
            pos.y + piece->blocks[i].dy,
        };
        if (p.x >= 0 && p.x < MAP_SIZE_X && p.y < MAP_SIZE_Y) {
            if (p.y >= 0 && map->block[p.y][p.x] != 0) {
                return true;
            }
        } else {
            return true;
        }
    }
    return false;
}

coord_t piece_get_max_dy(const piece_t* piece)
{
    coord_t max_dy = INT16_MIN;
    for (size_t i = 0; i < NUM_ELEMS(piece->blocks); i++) {
        max_dy = MAX(max_dy, piece->blocks[i].dy);
    }
    return max_dy;
}

void piece_rotate(piece_t* piece)
{
    for (size_t i = 0; i < NUM_ELEMS(piece->blocks); i++) {
        struct rel_pos rp = piece->blocks[i];
        piece->blocks[i].dy = rp.dx;
        piece->blocks[i].dx = -rp.dy;
    }
}
