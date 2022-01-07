#include "game.h"

#include <string.h>

#include "coop_sched.h"
#include "game/output.h"
#include "game/piece.h"
#include "util/delay.h"
#include "util/minicurses.h"
#include "util/prng.h"

typedef struct game_state {
    enum { piece_new, piece_falling, piece_landed, pause } fsm;
    game_map_t map;
    piece_t piece;
    pos_t pos;
    timeout_t timeout;
    uint16_t interval_ms;
} game_state_t;

static game_state_t game;

static game_cmd_t game_cmd;

static uint8_t stack[2000];
static coop_task_t task;

void game_send_cmd(game_cmd_t cmd)
{
    game_cmd = cmd;
}

static void remove_full_rows(game_map_t* map)
{
    bool full_rows_present = false;
    for (coord_t y = 0; y < MAP_SIZE_Y; y++) {
        bool row_full = true;
        for (coord_t x = 0; x < MAP_SIZE_X; x++) {
            if (map->block[y][x] == 0) {
                row_full = false;
                break;
            }
        }
        if (row_full) {
            // Flash the row white
            memset(map->block[y], mc_color_white, sizeof(map->block[y]));
        }
        full_rows_present |= row_full;
    }
    if (!full_rows_present) {
        return;
    }
    // Flash all full rows
    output_render(map);
    delay_ms(100);
    for (coord_t y = 0; y < MAP_SIZE_Y; y++) {
        // White color means full row.
        if (map->block[y][0] == mc_color_white) {
            if (y > 0) {
                // Move rows down
                memmove(map->block[1], map->block[0], y * sizeof(map->block[0]));
            }
            // Clear topmost row
            memset(map->block[0], 0, sizeof(map->block[0]));
        }
    }
    // Draw new map with removed rows
    output_render(map);
}

static void game_reset(void)
{
    game = (game_state_t){
        .fsm = piece_new,
        .interval_ms = 200,
    };
    game_cmd = cmd_none;
    prng_reseed();
}

static void game_init(void)
{
    mc_initscr();
    mc_setcursor(false);
    output_init();
    game_reset();
}

static void game_exit(void)
{
    mc_exitscr();
    mc_clear();
}

static pos_t delta_pos(pos_t delta)
{
    return (pos_t){game.pos.x + delta.x, game.pos.y + delta.y};
}

static void try_move(pos_t delta)
{
    pos_t pos_new = delta_pos(delta);
    if (!piece_collision(&game.piece, pos_new, &game.map)) {
        game.pos = pos_new;
    }
}

static void handle_cmd(void)
{
    if (game_cmd == cmd_none) {
        return;
    }
    if (game.fsm == pause) {
        if (game_cmd == cmd_pause) {
            game.fsm = piece_falling;
        }
        game_cmd = cmd_none;
        return;
    }

    switch (game_cmd) {
        case cmd_rotate:
            piece_rotate(&game.piece);
            break;
        case cmd_left:
            try_move((pos_t){-1, 0});
            break;
        case cmd_right:
            try_move((pos_t){1, 0});
            break;
        case cmd_down:
            try_move((pos_t){0, 1});
            break;
        case cmd_drop: {
            pos_t pos_new = game.pos;
            while (!piece_collision(&game.piece, pos_new, &game.map)) {
                game.pos = pos_new;
                pos_new = delta_pos((pos_t){0, 1});
            }
            game.fsm = piece_landed;
        } break;
        case cmd_pause:
            game.fsm = pause;
            break;
        default:
            break;
    }
    game_cmd = cmd_none;
}

static void game_render()
{
    static game_map_t render_map;
    render_map = game.map;
    piece_draw(&game.piece, game.pos, &render_map);
    output_render(&render_map);
}

void task_fn(void* arg)
{
    (void)arg;
    game_init();
    while (1) {
        switch (game.fsm) {
            case piece_new:
                game.piece = piece_get_random();
                game.pos = (pos_t){
                    .x = MAP_SIZE_X / 2 - 1,
                    .y = -piece_get_max_dy(&game.piece) - 1,
                };
                game.fsm = piece_falling;
                timeout_set(&game.timeout, game.interval_ms);
            case piece_falling:
                if (timeout_elapsed(&game.timeout)) {
                    timeout_set(&game.timeout, game.interval_ms);
                    if (piece_collision(&game.piece, delta_pos((pos_t){0, 1}), &game.map)) {
                        game.fsm = piece_landed;
                        break;
                    }
                    game.pos.y++;
                }
                handle_cmd();
                game_render();
                break;
            case piece_landed:
                piece_draw(&game.piece, game.pos, &game.map);
                remove_full_rows(&game.map);
                game.fsm = piece_new;
                break;
            case pause:
                handle_cmd();
                break;
        }
        sched_yield();
    }
    game_exit();
}

void game_start(void)
{
    sched_create_task(&task, &task_fn, NULL, stack, sizeof(stack));
}
