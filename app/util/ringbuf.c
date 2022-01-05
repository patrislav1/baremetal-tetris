#include "ringbuf.h"

#include "cmsis_gcc.h"

void ringbuf_init(ringbuf_t* r)
{
    r->ridx = 0;
    r->widx = 0;
    r->overflow_flag = false;
}

bool ringbuf_empty(ringbuf_t* ringbuf)
{
    return ringbuf->ridx == ringbuf->widx;
}

bool ringbuf_full(ringbuf_t* ringbuf)
{
    return !((ringbuf->widx - ringbuf->ridx + 1) & ringbuf->size_mask);
}

void ringbuf_push(ringbuf_t* r, char c)
{
    if (ringbuf_full(r)) {
        r->overflow_flag = true;
    } else {
        r->buf[r->widx & r->size_mask] = c;
        __DMB();
        r->widx++;
    }
}

char ringbuf_pop(ringbuf_t* r)
{
    char c = r->buf[r->ridx & r->size_mask];
    __DMB();
    r->ridx++;
    return c;
}
