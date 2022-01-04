#include "delay.h"

#include "stm32l433xx.h"

static volatile uint32_t tick = 0;

void SysTick_Handler(void)
{
    tick++;
}

void delay_init(void)
{
    SysTick_Config(SystemCoreClock / 1000);
}

void delay_ms(uint32_t ms)
{
    uint32_t val = tick + ms;
    while (((int)val - (int)tick) >= 0) {
        //        sched_yield();
    };
}
