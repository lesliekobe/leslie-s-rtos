#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

#define NULL     ((void*)0)
#define TRUE     1
#define FALSE    0

#define ALIGN(x, a)   ((x) + ((a) - 1)) & ~((a) - 1)
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

typedef enum {
    OK = 0,
    ERR_INVALID = -1,
    ERR_NO_MEM = -2,
    ERR_NO_TASK = -3,
    ERR_TIMEOUT = -4,
    ERR_BUSY = -5,
    ERR_IRQ = -6,
} status_t;

#endif /* TYPES_H */