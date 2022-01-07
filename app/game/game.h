#pragma once

typedef enum game_cmd {
    cmd_none,
    cmd_left,
    cmd_right,
    cmd_rotate,
    cmd_down,
    cmd_drop,
    cmd_pause,
} game_cmd_t;

void game_start(void);
void game_send_cmd(game_cmd_t cmd);
