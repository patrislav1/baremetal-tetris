#include "app.h"

#include "gpio.h"
#include "uart.h"

void app_main(void)
{
    uart_init();

    while (1) {
        LL_GPIO_SetOutputPin(LD4_GPIO_Port, LD4_Pin);
        LL_mDelay(100);
        LL_GPIO_ResetOutputPin(LD4_GPIO_Port, LD4_Pin);
        LL_mDelay(100);
        uart_write_string("HELLO World.\r\n");
    }
}