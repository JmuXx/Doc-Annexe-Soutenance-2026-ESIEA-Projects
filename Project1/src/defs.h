#ifndef DEFS_H
#define DEFS_H

#include <stdint.h>

typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;



#ifdef NDEBUG
    #define CHESS_ASSERT(expr) ((void)0)
#else
    #include <assert.h>
    #define CHESS_ASSERT(expr) assert(expr)
#endif

#endif
