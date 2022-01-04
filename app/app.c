#include "app.h"

#include <stdio.h>

#include "coop_sched.h"
#include "delay.h"
#include "gpio.h"
#include "uart.h"

static coop_task_t mytask1, mytask2;
static uint8_t mystack1[2000];
static uint8_t mystack2[2000];

static void test_task_1(void* arg)
{
    size_t l = 100;
    while (l--) {
        ((l & 1) ? LL_GPIO_SetOutputPin : LL_GPIO_ResetOutputPin)(LD4_GPIO_Port, LD4_Pin);
        delay_ms(100);
    }
    printf("Task1 Watermark: %u\r\n", get_stack_watermark(&mytask1));
}

static void test_task_2(void* arg)
{
    size_t l = 150;
    while (l--) {
        printf("Countdown %d\r\n", l);
        delay_ms(110);
    }
    printf("Task2 Watermark: %u\r\n", get_stack_watermark(&mytask2));
}

void app_main(void)
{
    uart_init();
    delay_init();

    sched_init();
    sched_create_task(&mytask1, test_task_1, NULL, mystack1, sizeof(mystack1));
    sched_create_task(&mytask2, test_task_2, NULL, mystack2, sizeof(mystack2));

    while (1) {
        printf("Main Loop.\r\n");
        delay_ms(1000);
    }
}