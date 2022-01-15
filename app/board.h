#pragma once

#if defined STM32L433xx

#include "stm32l433xx.h"
#define CONSOLE_UART USART2
#define CONSOLE_IRQ_HANDLER USART2_IRQHandler

#elif defined STM32F103xB

#include "stm32f103xb.h"
#define CONSOLE_USB

#endif
