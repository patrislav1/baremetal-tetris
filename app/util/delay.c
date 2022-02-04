#include "delay.h"

#include "board.h"
#include "coop_sched.h"

static volatile uint32_t tick = 0;

extern void HAL_IncTick(void);

void SysTick_Handler(void)
{
    tick++;
#ifdef STM32F401xC
    HAL_IncTick();
#endif
}

void delay_init(void)
{
    SysTick_Config(SystemCoreClock / 1000);
}

void delay_ms(uint32_t ms)
{
    uint32_t val = tick + ms;
    while (((int)val - (int)tick) >= 0) {
        sched_yield();
    };
}

uint32_t get_ms(void)
{
    return tick;
}

void timeout_set(timeout_t* t, uint32_t ms)
{
    *t = tick + ms;
}

bool timeout_elapsed(timeout_t* t)
{
    return ((int)(*t) - (int)tick) <= 0;
}
