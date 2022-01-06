#pragma once

#include <stdbool.h>
#include <stdint.h>

void delay_init(void);
void delay_ms(uint32_t ms);

uint32_t get_ms(void);

typedef uint32_t timeout_t;
void timeout_set(timeout_t* t, uint32_t ms);
bool timeout_elapsed(timeout_t* t);