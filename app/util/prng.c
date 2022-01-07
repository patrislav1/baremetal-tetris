#include "prng.h"

#include <stdint.h>

#include "macros.h"
#include "stm32l4xx.h"
#include "util/delay.h"

#define N 624
#define M 397

static uint32_t vector[N];

static void prng_update(uint32_t* const p)
{
    static const uint32_t A[2] = {0, 0x9908B0DF};

    int i;
    for (i = 0; i < N - M; i++) {
        p[i] =
            p[i + (M)] ^ (((p[i] & 0x80000000) | (p[i + 1] & 0x7FFFFFFF)) >> 1) ^ A[p[i + 1] & 1];
    }
    for (; i < N - 1; i++) {
        p[i] = p[i + (M - N)] ^ (((p[i] & 0x80000000) | (p[i + 1] & 0x7FFFFFFF)) >> 1) ^
               A[p[i + 1] & 1];
    }
    p[N - 1] = p[M - 1] ^ (((p[N - 1] & 0x80000000) | (p[0] & 0x7FFFFFFF)) >> 1) ^ A[p[0] & 1];
}

void prng_seed(uint32_t seed)
{
    const uint32_t mult = 1812433253ul;
    int i;

    for (i = 0; i < NUM_ELEMS(vector); i++) {
        vector[i] = seed;
        seed = mult * (seed ^ (seed >> 30)) + (i + 1);
    }
}

uint32_t prng_next()
{
    static int idx = N;

    uint32_t e;

    if (idx >= N) {
        prng_update(vector);
        idx = 0;
    }

    e = vector[idx++];
    e ^= (e >> 11); /* Tempering */
    e ^= (e << 7) & 0x9D2C5680;
    e ^= (e << 15) & 0xEFC60000;
    e ^= (e >> 18);

    return e;
}

void prng_reseed(void)
{
    prng_seed(get_ms() ^ SysTick->VAL);
    for (int i = 0; i < SysTick->VAL % 2000; i++) {
        prng_next();
    }
}
