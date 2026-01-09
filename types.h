#ifndef TYPES_H
#define TYPES_H
#include <stddef.h>
#include <stdint.h>

#define GRAVITY 1
#define DIM_WINDOW 800

typedef int32_t i32;
typedef uint32_t u32;
typedef float f32;
typedef double f64;
typedef size_t usize;

typedef struct
{

  f64 x, y;

} vector;

#endif
