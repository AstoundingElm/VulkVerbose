#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#define VK_USE_PLATFORM_XCB_KHR
#include <vulkan/vulkan.h>

#define ArraySize(arr) sizeof((arr)) / sizeof((arr[0]))
typedef char b8;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint64_t u64;
typedef uint8_t u8;
typedef float f32;
typedef double f64;
#define PINLINE static inline

#ifndef MAX
#define MAX(x,y) ((x)>(y)?(x):(y))
#endif

#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif

#define KASSERT_MSG(expr, message)                                        \
{                                                                     \
if (expr) {                                                       \
} else {                                                          \
printf("%s, %s, %d, ", message,  __FILE__, __LINE__); \
__builtin_trap();                                                 \
}                                                                 \
}

