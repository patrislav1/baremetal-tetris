#include "app.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "coop_sched.h"
#include "game/output.h"
#include "game/piece.h"
#include "gpio.h"
#include "macros.h"
#include "util/delay.h"
#include "util/minicurses.h"
#include "util/prng.h"
#include "util/uart.h"

static void led_test_fn(void* arg)
{
    size_t l = 100;
    while (l--) {
        ((l & 1) ? LL_GPIO_SetOutputPin : LL_GPIO_ResetOutputPin)(LD4_GPIO_Port, LD4_Pin);
        // delay_ms() will implicitly call sched_yield()
        delay_ms(100);
    }
}

static enum {
    cmd_none,
    cmd_left,
    cmd_right,
    cmd_rotate,
    cmd_down,
    cmd_drop,
} game_cmd = cmd_none;

static void input_task(void* arg)
{
    while (1) {
        int c = mc_getch();
        switch (c) {
            case key_up:
                game_cmd = cmd_rotate;
                break;
            case key_down:
                game_cmd = cmd_down;
                break;
            case key_left:
                game_cmd = cmd_left;
                break;
            case key_right:
                game_cmd = cmd_right;
                break;
            case ' ':
                game_cmd = cmd_drop;
                break;
            default:
                break;
        }
    }
}

static struct {
    const char* name;
    coop_task_fn_t task_fn;
    coop_task_t task;
    uint8_t stack[1000];
} tasks[] = {
    {"LED", led_test_fn},
    {"input", input_task},
};

static game_map_t screen = {0}, screen_current;

static void mc_test(void)
{
    mc_initscr();
    mc_setcursor(false);
    output_init();
    enum { piece_new, piece_falling, piece_landed } state = piece_new;
    piece_t piece;
    timeout_t to;
    uint16_t interval_ms = 250;
    while (1) {
        switch (state) {
            case piece_new:
                piece = piece_get_random();
                pos_t pos = {
                    .x = MAP_SIZE_X / 2 - 1,
                    .y = -piece_get_max_dy(&piece) - 1,
                };
                state = piece_falling;
                timeout_set(&to, interval_ms);
            case piece_falling:
                if (timeout_elapsed(&to)) {
                    timeout_set(&to, interval_ms);
                    if (piece_collision(&piece, (pos_t){pos.x, pos.y + 1}, &screen)) {
                        state = piece_landed;
                        break;
                    }
                    pos.y++;
                }
                switch (game_cmd) {
                    pos_t pos_new;
                    case cmd_rotate:
                        piece_rotate(&piece);
                        break;
                    case cmd_left:
                        pos_new = (pos_t){pos.x - 1, pos.y};
                        if (!piece_collision(&piece, pos_new, &screen)) {
                            pos = pos_new;
                        }
                        break;
                    case cmd_right:
                        pos_new = (pos_t){pos.x + 1, pos.y};
                        if (!piece_collision(&piece, pos_new, &screen)) {
                            pos = pos_new;
                        }
                        break;
                    case cmd_down:
                        pos_new = (pos_t){pos.x, pos.y + 1};
                        if (!piece_collision(&piece, pos_new, &screen)) {
                            pos = pos_new;
                        }
                        break;
                    case cmd_drop:
                        pos_new = pos;
                        while (!piece_collision(&piece, pos_new, &screen)) {
                            pos = pos_new;
                            pos_new = (pos_t){pos_new.x, pos_new.y + 1};
                        }
                        break;
                    default:
                        break;
                }
                game_cmd = cmd_none;
                screen_current = screen;
                piece_draw(&piece, pos, &screen_current);
                output_render(&screen_current);
                break;
            case piece_landed:
                piece_draw(&piece, pos, &screen);
                screen = screen_current;
                state = piece_new;
                break;
        }
        sched_yield();
    }
    mc_exitscr();
    mc_clear();
}

void app_main(void)
{
    uart_init();
    delay_init();

    sched_init();

    for (size_t k = 0; k < sizeof(tasks) / sizeof(tasks[0]); k++) {
        sched_create_task(&tasks[k].task,
                          tasks[k].task_fn,
                          NULL,
                          tasks[k].stack,
                          sizeof(tasks[k].stack));
    }

    prng_seed(get_ms() ^ SysTick->VAL);
    for (int i = 0; i < SysTick->VAL % 2000; i++) {
        prng_next();
    }

    mc_test();
    while (1) {
        char c;
        // read() will implicitly call sched_yield()
        if (read(STDIN_FILENO, &c, sizeof(c)) == sizeof(c)) {
            printf("received: %c\r\n", c);
        }
        if (c == 'w') {
            for (size_t k = 0; k < sizeof(tasks) / sizeof(tasks[0]); k++) {
                size_t wm = get_stack_watermark(&tasks[k].task);
                printf("Stack watermark of %s task: %u\r\n", tasks[k].name, wm);
            }
        }
    }
}