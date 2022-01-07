#pragma once

#include <stdint.h>

void prng_seed(uint32_t seed);
uint32_t prng_next();
void prng_reseed(void);