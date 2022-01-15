#include "app.h"

#include <unistd.h>

#include "coop_sched.h"
#include "game/game.h"
#include "game/input.h"
#include "gpio.h"
#include "util/delay.h"
#include "util/uart.h"

void app_main(void)
{
    uart_init();
    delay_init();
    sched_init();

    input_start();
    game_start();

    int k = 0;
    while (1) {
        bool led_on = (k == 0 || k == 2);
        (led_on ? LL_GPIO_ResetOutputPin : LL_GPIO_SetOutputPin)(LED_GPIO_Port, LED_Pin);
        k = (k + 1) % 10;
        delay_ms(100);
    }
}
