#pragma once

#include "types.h"

// void memcpy(void *dest, const void *src, u32 size);
void memset32(void *dest, u32 filler, u32 size);
u8 *memsearch(u8 *startPos, const void *pattern, u32 size, u32 patternSize);