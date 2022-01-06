#include "app.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "coop_sched.h"
#include "game/output.h"
#include "gpio.h"
#include "macros.h"
#include "util/delay.h"
#include "util/minicurses.h"
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

static void fp_test_fn(void* arg)
{
    static float f = 0.f;
    while (f < 10.f) {
        //        printf("Value: %g\r\n", f);
        f += 0.1f;
        // delay_ms() will implicitly call sched_yield()
        delay_ms(110);
    }
}

static struct {
    const char* name;
    coop_task_fn_t task_fn;
    coop_task_t task;
    uint8_t stack[1000];
} tasks[] = {
    {"LED", led_test_fn},
    {"FP", fp_test_fn},
};

static game_map_t screen = {0};

static void mc_test(void)
{
    mc_initscr();
    mc_setcursor(false);
    output_init();
    const char* tmp = "Test";
    mc_color_t fg = mc_color_white;
    mc_color_t bg = mc_color_black;
    mc_attr_t attr = mc_attr_normal;
    bool quit = false;
    coord_t x = 0, y = 0;
    while (!quit) {
#if 0
        mc_move(x, y);
        mc_set_fg(fg);
        mc_set_bg(bg);
        mc_setattr(attr);
        mc_putch(mc_sym_hline);
        mc_putstr(tmp);
        mc_putch(mc_sym_hline);
#endif
        screen.block[y][x] = fg;
        output_render(&screen);
        int c = mc_getch();
        switch (c) {
            case key_up:
                y--;
                y = BOUND(y, 0, MAP_SIZE_Y - 1);
                break;
            case key_down:
                y++;
                y = BOUND(y, 0, MAP_SIZE_Y - 1);
                break;
            case key_left:
                x--;
                x = BOUND(x, 0, MAP_SIZE_X - 1);
                break;
            case key_right:
                x++;
                x = BOUND(x, 0, MAP_SIZE_X - 1);
                break;
            case 'f':
                fg += 1;
                fg %= 8;
                break;
            case 'b':
                bg += 1;
                bg %= 8;
                break;
            case 'q':
                quit = true;
                break;
            case 'a':
                memset(&screen, 0, sizeof(screen));
                //                attr ^= mc_attr_blink | mc_attr_italic;
                break;
            default:
                break;
        }
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