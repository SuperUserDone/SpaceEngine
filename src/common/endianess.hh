#pragma once

#include <intrin.h>
#include <stdint.h>

#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || defined(__BIG_ENDIAN__)
#define BIG_ENDIAN
#elif defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || defined(__LITTLE_ENDIAN__)
#define LITTLE_ENDIAN
#endif

#ifdef LITTLE_ENDIAN

#define host_to_little(in) in

#endif