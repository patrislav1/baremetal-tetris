#include "input.h"

#include <stddef.h>

#include "coop_sched.h"
#include "game.h"
#include "macros.h"
#include "util/minicurses.h"

static uint8_t stack[2000];
static coop_task_t task;

static const struct key_binding {
    uint8_t key;
    game_cmd_t cmd;
} key_bindings[] = {
    {key_up, cmd_rotate},
    {key_down, cmd_down},
    {key_left, cmd_left},
    {key_right, cmd_right},
    {' ', cmd_drop},
};

static void task_fn(void* arg)
{
    while (1) {
        int c = mc_getch();
        for (size_t i = 0; i < NUM_ELEMS(key_bindings); i++) {
            if (c == key_bindings[i].key) {
                game_send_cmd(key_bindings[i].cmd);
            }
        }
    }
}

void input_start(void)
{
    sched_create_task(&task, &task_fn, NULL, stack, sizeof(stack));
}
