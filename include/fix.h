#ifndef __FIX_H__
#define __FIX_H__

#include <stdint.h>
#include <stdlib.h>

#define FIX_ONE (1UL << 10)
#define FIX_ZERO (0x00000000)

typedef int16_t fix16_t;

__attribute__((always_inline)) fix16_t fix_mul(fix16_t x, fix16_t y)
{
    int32_t res = (int32_t)x * y;
    return (fix16_t)(res >> 10);
}

static inline fix16_t i16_to_fix(int16_t x)
{
    if (x >= 0)
    {
        return x >> 2;
    }
    else
    {
        return -(abs(x) >> 2);
    }
}

#endif
