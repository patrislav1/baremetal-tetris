#include "app.h"

#include "gpio.h"

void app_main(void)
{
    while (1) {
        LL_GPIO_SetOutputPin(LD4_GPIO_Port, LD4_Pin);
        LL_mDelay(100);
        LL_GPIO_ResetOutputPin(LD4_GPIO_Port, LD4_Pin);
        LL_mDelay(100);
    }
}