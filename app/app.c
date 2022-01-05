#include "app.h"

#include <stdio.h>
#include <unistd.h>

#include "coop_sched.h"
#include "delay.h"
#include "gpio.h"
#include "uart.h"

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
        printf("Value: %g\r\n", f);
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