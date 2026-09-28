#ifndef common_h
#define common_h

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <iso646.h>

typedef int8_t  i8,   *i8Ptr;
typedef int16_t i16,  *i16Ptr;
typedef int32_t i32,  *i32Ptr;
typedef int64_t i64,  *i64Ptr;

typedef uint8_t  u8,  *u8Ptr;
typedef uint16_t u16, *u16Ptr;
typedef uint32_t u32, *u32Ptr;
typedef uint64_t u64, *u64Ptr;

typedef size_t usize;
typedef void * ptr;

typedef char byte, *bytePtr;

#define null NULL

// used to annotate when a pointer field is intended to be used as a list.
#define List(x) x*

// included last, as these depend on the typedefs above
#include "mem.h"
#include "panic.h"

#endif