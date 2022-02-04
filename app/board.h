#pragma once

#if defined STM32L433xx

#include "stm32l433xx.h"

#define LED_INVERTED

#elif defined STM32F103xB

#include "stm32f103xb.h"

#elif defined STM32F401xC

#include "stm32f401xc.h"

#endif
