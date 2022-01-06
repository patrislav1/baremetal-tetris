#pragma once

#define NUM_ELEMS(x) (sizeof(x) / sizeof(x[0]))

#define ATTR_PACKED __attribute__((packed))

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#define BOUND(v, a, b) (MIN(MAX(v, a), b))
