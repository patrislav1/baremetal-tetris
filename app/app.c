#include "app.h"

#include <unistd.h>

#include "coop_sched.h"
#include "game/game.h"
#include "game/input.h"
#include "util/delay.h"
#include "util/uart.h"

void app_main(void)
{
    uart_init();
    delay_init();
    sched_init();

    input_start();
    game_start();

    while (1) {
        sched_yield();
    }
}