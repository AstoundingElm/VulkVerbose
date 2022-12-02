#pragma once
#include "defines.h"
typedef union vec3_u {
        // An array of x, y, z
        f32 elements[3];
        struct {
                union {
                        // The first element.
                        f32 x, r, s, u;
                };
                union {
                        // The second element.
                        f32 y, g, t, v;
                };
                union {
                        // The third element.
                        f32 z, b, p, w;
                };
        };
} vec3;

typedef union mat4_u {
        f32 data[16];
} mat4;


