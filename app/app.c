#include "app.h"

#include <stdio.h>

#include "coop_sched.h"
#include "delay.h"
#include "gpio.h"
#include "uart.h"

void app_main(void)
{
    uart_init();
    delay_init();

    int i = 0;
    while (1) {
        LL_GPIO_SetOutputPin(LD4_GPIO_Port, LD4_Pin);
        delay_ms(100);
        LL_GPIO_ResetOutputPin(LD4_GPIO_Port, LD4_Pin);
        delay_ms(100);
        printf("HELLO World %d.\r\n", i++);
    }
}