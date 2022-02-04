#pragma once

#if defined STM32L433xx

#include "stm32l433xx.h"

#define LED_INVERTED

#elif defined STM32F103xB

#include "stm32f103xb.h"

#elif defined STM32F401xE

#include "stm32f401xe.h"

#endif
