#include "input.h"

#include <stddef.h>

#include "coop_sched.h"
#include "macros.h"
#include "util/minicurses.h"

static uint8_t stack[2000];
static coop_task_t task;

const key_binding_t key_bindings[] = {
    {key_up, "🠉", cmd_rotate, "rotate"},
    {key_left, "🠈", cmd_left, "move left"},
    {key_right, "🠊", cmd_right, "move right"},
    {key_down, "🠋", cmd_down, "move down"},
    {' ', "space", cmd_drop, "drop"},
    {'p', "p", cmd_pause, "pause"},
};

const size_t num_key_bindings = NUM_ELEMS(key_bindings);

static void task_fn(void* arg)
{
    while (1) {
        int c = mc_getch();
        size_t i;
        for (i = 0; i < num_key_bindings; i++) {
            if (c == key_bindings[i].key) {
                game_send_cmd(key_bindings[i].cmd);
                break;
            }
        }
        if (i == NUM_ELEMS(key_bindings)) {
            game_send_cmd(cmd_other_key);
        }
    }
}

void input_start(void)
{
    sched_create_task(&task, &task_fn, NULL, stack, sizeof(stack));
}
