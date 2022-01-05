#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct ringbuf {
    volatile char* buf;
    uint16_t size_mask;
    volatile uint16_t ridx;
    volatile uint16_t widx;
    volatile bool overflow_flag;
} ringbuf_t;

#define RINGBUF_INIT(x)                      \
    {                                        \
        .buf = x, .size_mask = sizeof(x) - 1 \
    }

void ringbuf_init(ringbuf_t* r);
bool ringbuf_empty(ringbuf_t* ringbuf);
bool ringbuf_full(ringbuf_t* ringbuf);
void ringbuf_push(ringbuf_t* r, char c);
char ringbuf_pop(ringbuf_t* r);
