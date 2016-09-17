#include "memory.h"

// void memcpy(void *dest, const void *src, u32 size)
// {
//     u8 *destc = (u8 *)dest;
//     const u8 *srcc = (const u8 *)src;
// 
//     for(u32 i = 0; i < size; i++)
//         destc[i] = srcc[i];
// }

void memset32(void *dest, u32 filler, u32 size)
{
    u32 *dest32 = (u32 *)dest;

    for (u32 i = 0; i < size / 4; i++)
        dest32[i] = filler;
}

u8 *memsearch(u8 *startPos, const void *pattern, u32 size, u32 patternSize)
{
    const u8 *patternc = (const u8 *)pattern;

    //Preprocessing
    u32 table[256];

    for(u32 i = 0; i < 256; ++i)
        table[i] = patternSize + 1;
    for(u32 i = 0; i < patternSize; ++i)
        table[patternc[i]] = patternSize - i;

    //Searching
    u32 j = 0;

    while(j <= size - patternSize)
    {
        if(memcmp(patternc, startPos + j, patternSize) == 0)
            return startPos + j;
        j += table[startPos[j + patternSize]];
    }

    return NULL;
}